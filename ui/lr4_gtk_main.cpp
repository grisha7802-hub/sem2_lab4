#include <gtk/gtk.h>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "ArraySequence.hpp"
#include "LazySequence.hpp"
#include "OnlineStatistics.hpp"
#include "Stream.hpp"

static int squareInt(int x) { return x * x; }
static int sumInt(int a, int b) { return a + b; }
static bool isEvenInt(int x) { return x % 2 == 0; }
static int parseInt(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::invalid_argument("parse int");
    }
}

struct Lr4UiContext {
    GtkWidget* countEntry;
    GtkWidget* indexEntry;
    GtkWidget* outputLabel;
    Stream<int> fib;
    Stream<int> finite;
};

static int ReadIntegerFromEntry(GtkWidget* entryWidget, bool& isValid) {
    isValid = false;
    const char* rawText = gtk_entry_get_text(GTK_ENTRY(entryWidget));
    char* parseEnd = nullptr;
    long parsedValue = strtol(rawText ? rawText : "", &parseEnd, 10);
    if (parseEnd != rawText && *parseEnd == '\0') {
        isValid = true;
        return static_cast<int>(parsedValue);
    }
    return 0;
}

template<class T>
static std::string SequenceToString(const Sequence<T>& sequence) {
    std::ostringstream output;
    output << "[";
    for (int index = 0; index < sequence.GetSize(); ++index) {
        output << sequence.Get(index);
        if (index + 1 < sequence.GetSize()) output << ", ";
    }
    output << "]";
    return output.str();
}

template<class First, class Second>
static std::string PairSequenceToString(const Sequence<std::pair<First, Second>>& sequence) {
    std::ostringstream output;
    output << "[";
    for (int index = 0; index < sequence.GetSize(); ++index) {
        output << "(" << sequence.Get(index).first << ", " << sequence.Get(index).second << ")";
        if (index + 1 < sequence.GetSize()) output << ", ";
    }
    output << "]";
    return output.str();
}

static void ShowUiState(Lr4UiContext* context, const std::string& message) {
    gtk_label_set_text(GTK_LABEL(context->outputLabel), message.c_str());
}

static int ReadPositiveCountOrReport(Lr4UiContext* context, int fallbackValue) {
    bool isCountValid = false;
    int count = ReadIntegerFromEntry(context->countEntry, isCountValid);
    if (!isCountValid) count = fallbackValue;
    if (count < 0) {
        ShowUiState(context, "Ошибка: Count не может быть отрицательным");
        return -1;
    }
    return count;
}

static void OnFibonacciClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    int count = ReadPositiveCountOrReport(context, 10);
    if (count < 0) return;

    auto values = context->fib.Take(count);
    ShowUiState(context, "Fibonacci => " + SequenceToString(values));
}

static void OnFibonacciSquaresClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    int count = ReadPositiveCountOrReport(context, 10);
    if (count < 0) return;

    auto squared = context->fib.Map<int>(squareInt);
    auto values = squared.Take(count);
    ShowUiState(context, "Fibonacci squares => " + SequenceToString(values));
}

static void OnFiniteEvensClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    auto evens = context->finite.Where(isEvenInt);
    auto values = evens.Take(3);
    ShowUiState(context, "Finite evens => " + SequenceToString(values));
}

static void OnFiniteSumClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    int sum = context->finite.Reduce<int>(sumInt, 0);
    ShowUiState(context, "Finite reduce sum => " + std::to_string(sum));
}

static void OnFibonacciGetClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    bool isIndexValid = false;
    int index = ReadIntegerFromEntry(context->indexEntry, isIndexValid);
    if (!isIndexValid || index < 0) {
        return ShowUiState(context, "Ошибка: Index должен быть неотрицательным целым числом");
    }

    try {
        int value = context->fib.Get(index);
        ShowUiState(context, "Fibonacci[" + std::to_string(index) + "] => " + std::to_string(value));
    } catch (const std::exception& exceptionObject) {
        ShowUiState(context, std::string("Ошибка: ") + exceptionObject.what());
    }
}

static void OnZipClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    int secondRaw[4] = {10, 20, 30, 40};
    auto second = Stream<int>::FromArray(secondRaw, 4);
    auto zipped = context->finite.Zip(second);
    auto values = zipped.Take(4);
    ShowUiState(context, "Zip finite with [10,20,30,40] => " + PairSequenceToString(values));
}

static void OnLazySubsequenceClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    int finiteRaw[6] = {1, 2, 3, 4, 5, 6};
    LazySequence<int> lazyFinite(finiteRaw, 6);
    LazySequence<int>* subsequence = lazyFinite.GetSubsequence(1, 3);

    ArraySequence<int> values;
    for (int index = 0; index < 3; ++index) {
        values.Append(subsequence->Get(index));
    }
    delete subsequence;

    ShowUiState(context, "LazySequence subsequence 1..3 => " + SequenceToString(values));
}

static void OnManualStreamClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    const char* rawText = gtk_entry_get_text(GTK_ENTRY(context->countEntry));

    try {
        ReadOnlyStream<int> stream(std::string(rawText ? rawText : ""), parseInt);
        ArraySequence<int> values;
        while (!stream.IsEndOfStream()) {
            values.Append(stream.Read());
        }
        ShowUiState(context, "Manual stream read => " + SequenceToString(values));
    } catch (const std::exception& exceptionObject) {
        ShowUiState(context, std::string("Ошибка: ") + exceptionObject.what());
    }
}

static void OnStatisticsClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<Lr4UiContext*>(userData);
    const char* rawText = gtk_entry_get_text(GTK_ENTRY(context->countEntry));

    try {
        ReadOnlyStream<int> stream(std::string(rawText ? rawText : ""), parseInt);
        OnlineStatistics<int> stats;
        stats.Consume(stream);
        ShowUiState(context,
                    "Online stats => count=" + std::to_string(stats.GetCount())
                    + " sum=" + std::to_string(stats.GetSum())
                    + " min=" + std::to_string(stats.GetMin())
                    + " max=" + std::to_string(stats.GetMax())
                    + " mean=" + std::to_string(stats.GetMean())
                    + " median=" + std::to_string(stats.GetMedian())
                    + " window3mean=" + std::to_string(stats.GetWindowMean(3)));
    } catch (const std::exception& exceptionObject) {
        ShowUiState(context, std::string("Ошибка: ") + exceptionObject.what());
    }
}

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    int fibSeedRaw[2] = {0, 1};
    ArraySequence<int> fibSeed(fibSeedRaw, 2);
    auto fib = Stream<int>::Recurrence(
        fibSeed,
        [](const Sequence<int>& prefix) {
            int n = prefix.GetSize();
            return prefix.Get(n - 1) + prefix.Get(n - 2);
        },
        SequenceLength::Infinity()
    );

    int finiteRaw[6] = {1, 2, 3, 4, 5, 6};
    auto finite = Stream<int>::FromArray(finiteRaw, 6);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "LR4 Stream GTK UI");
    gtk_window_set_default_size(GTK_WINDOW(window), 1060, 560);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget* rootBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(rootBox), 10);

    GtkWidget* inputRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget* countEntry = gtk_entry_new();
    GtkWidget* indexEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(countEntry), "Count or manual stream: 4 1 7 3");
    gtk_entry_set_placeholder_text(GTK_ENTRY(indexEntry), "Index");
    gtk_box_pack_start(GTK_BOX(inputRow), countEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inputRow), indexEntry, FALSE, FALSE, 0);

    GtkWidget* firstButtonRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget* secondButtonRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    GtkWidget* outputLabel = gtk_label_new("-");
    gtk_label_set_xalign(GTK_LABEL(outputLabel), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(outputLabel), TRUE);

    GtkWidget* outputFrame = gtk_frame_new("Result");
    gtk_container_add(GTK_CONTAINER(outputFrame), outputLabel);

    auto* context = new Lr4UiContext{countEntry, indexEntry, outputLabel, fib, finite};

    auto addButton = [&](GtkWidget* rowWidget, const char* title, GCallback callback) {
        GtkWidget* button = gtk_button_new_with_label(title);
        g_signal_connect(button, "clicked", callback, context);
        gtk_box_pack_start(GTK_BOX(rowWidget), button, TRUE, TRUE, 0);
    };

    addButton(firstButtonRow, "Fibonacci", G_CALLBACK(OnFibonacciClicked));
    addButton(firstButtonRow, "Fibonacci (^2)", G_CALLBACK(OnFibonacciSquaresClicked));
    addButton(firstButtonRow, "Fibonacci[index]", G_CALLBACK(OnFibonacciGetClicked));
    addButton(firstButtonRow, "Finite evens", G_CALLBACK(OnFiniteEvensClicked));

    addButton(secondButtonRow, "Finite sum", G_CALLBACK(OnFiniteSumClicked));
    addButton(secondButtonRow, "Zip", G_CALLBACK(OnZipClicked));
    addButton(secondButtonRow, "Lazy subsequence", G_CALLBACK(OnLazySubsequenceClicked));
    addButton(secondButtonRow, "Manual stream", G_CALLBACK(OnManualStreamClicked));
    addButton(secondButtonRow, "Online stats", G_CALLBACK(OnStatisticsClicked));

    gtk_box_pack_start(GTK_BOX(rootBox), inputRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), firstButtonRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), secondButtonRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), outputFrame, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), rootBox);
    gtk_widget_show_all(window);

    ShowUiState(context, "Интерфейс LR4 готов");
    gtk_main();

    delete context;
    return 0;
}
