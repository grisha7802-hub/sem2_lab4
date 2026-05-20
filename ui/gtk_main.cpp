#include <gtk/gtk.h>
#include <sstream>
#include "Stack.hpp"

struct StackUiContext {
    Stack<int> stack;
    GtkWidget* valueEntry;
    GtkWidget* rangeStartEntry;
    GtkWidget* rangeEndEntry;
    GtkWidget* outputLabel;
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

static std::string StackToString(const Stack<int>& stack) {
    std::ostringstream output;
    output << "[";
    for (int elementIndex = 0; elementIndex < stack.GetSize(); ++elementIndex) {
        output << stack.Raw().Get(elementIndex);
        if (elementIndex + 1 < stack.GetSize()) {
            output << ", ";
        }
    }
    output << "]";
    return output.str();
}

static void ShowUiState(StackUiContext* context, const std::string& message) {
    std::ostringstream output;
    output << message
           << "\nStack = " << StackToString(context->stack)
           << "\nSize = " << context->stack.GetSize();
    gtk_label_set_text(GTK_LABEL(context->outputLabel), output.str().c_str());
}

static bool IsEvenNumber(const int& value) { return value % 2 == 0; }
static int SquareNumber(const int& value) { return value * value; }
static int SumTwoNumbers(const int& leftValue, const int& rightValue) { return leftValue + rightValue; }

static void OnPushClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    bool isValueValid = false;
    int valueToPush = ReadIntegerFromEntry(context->valueEntry, isValueValid);
    if (!isValueValid) {
        return ShowUiState(context, "Ошибка: Value должен быть целым числом");
    }
    context->stack.Push(valueToPush);
    ShowUiState(context, "Push выполнен");
}

static void OnTryPopClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    auto popResult = context->stack.TryPop();
    ShowUiState(context, popResult.IsSome() ? "TryPop: Some" : "TryPop: None");
}

static void OnTryPeekClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    auto peekResult = context->stack.TryPeek();
    ShowUiState(context, peekResult.IsSome() ? "TryPeek: Some" : "TryPeek: None");
}

static void OnMapClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    auto mappedStack = context->stack.Map<int>(SquareNumber);
    ShowUiState(context, "Map (^2) => " + StackToString(mappedStack));
}

static void OnWhereClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    auto filteredStack = context->stack.Where(IsEvenNumber);
    ShowUiState(context, "Where (even) => " + StackToString(filteredStack));
}

static void OnReduceClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    int reducedValue = context->stack.Reduce(SumTwoNumbers, 0);
    ShowUiState(context, "Reduce (sum) => " + std::to_string(reducedValue));
}

static void OnConcatClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    int appendedValues[2] = {9, 10};
    Stack<int> suffixStack(appendedValues, 2);
    context->stack = context->stack.Concat(suffixStack);
    ShowUiState(context, "Concat with [9, 10]");
}

static void OnSubStackClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    bool isStartValid = false;
    bool isEndValid = false;
    int startIndex = ReadIntegerFromEntry(context->rangeStartEntry, isStartValid);
    int endIndex = ReadIntegerFromEntry(context->rangeEndEntry, isEndValid);
    if (!isStartValid || !isEndValid) {
        return ShowUiState(context, "Ошибка: Start/End должны быть целыми");
    }

    try {
        auto extractedSubstack = context->stack.GetSubStack(startIndex, endIndex);
        ShowUiState(context, "SubStack => " + StackToString(extractedSubstack));
    } catch (const std::exception& exceptionObject) {
        ShowUiState(context, std::string("Ошибка: ") + exceptionObject.what());
    }
}

static void OnFindSubStackClicked(GtkWidget*, gpointer userData) {
    auto* context = static_cast<StackUiContext*>(userData);
    bool isFirstValid = false;
    bool isSecondValid = false;
    int firstPatternElement = ReadIntegerFromEntry(context->rangeStartEntry, isFirstValid);
    int secondPatternElement = ReadIntegerFromEntry(context->rangeEndEntry, isSecondValid);
    if (!isFirstValid || !isSecondValid) {
        return ShowUiState(context, "Ошибка: Pattern[0]/Pattern[1] должны быть целыми");
    }

    int patternValues[2] = {firstPatternElement, secondPatternElement};
    Stack<int> patternStack(patternValues, 2);
    int foundPosition = context->stack.FindSubStack(patternStack);
    ShowUiState(context, "FindSubStack => " + std::to_string(foundPosition));
}

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "LR3 Stack GTK UI");
    gtk_window_set_default_size(GTK_WINDOW(window), 1060, 560);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget* rootBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(rootBox), 10);

    GtkWidget* inputRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget* valueEntry = gtk_entry_new();
    GtkWidget* rangeStartEntry = gtk_entry_new();
    GtkWidget* rangeEndEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(valueEntry), "Value");
    gtk_entry_set_placeholder_text(GTK_ENTRY(rangeStartEntry), "Start / Pattern[0]");
    gtk_entry_set_placeholder_text(GTK_ENTRY(rangeEndEntry), "End / Pattern[1]");
    gtk_box_pack_start(GTK_BOX(inputRow), valueEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inputRow), rangeStartEntry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inputRow), rangeEndEntry, FALSE, FALSE, 0);

    GtkWidget* firstButtonRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget* secondButtonRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    GtkWidget* outputLabel = gtk_label_new("-");
    gtk_label_set_xalign(GTK_LABEL(outputLabel), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(outputLabel), TRUE);

    GtkWidget* outputFrame = gtk_frame_new("Result");
    gtk_container_add(GTK_CONTAINER(outputFrame), outputLabel);

    auto* context = new StackUiContext{Stack<int>(), valueEntry, rangeStartEntry, rangeEndEntry, outputLabel};

    auto addButton = [&](GtkWidget* rowWidget, const char* title, GCallback callback) {
        GtkWidget* button = gtk_button_new_with_label(title);
        g_signal_connect(button, "clicked", callback, context);
        gtk_box_pack_start(GTK_BOX(rowWidget), button, TRUE, TRUE, 0);
    };

    addButton(firstButtonRow, "Push", G_CALLBACK(OnPushClicked));
    addButton(firstButtonRow, "TryPop", G_CALLBACK(OnTryPopClicked));
    addButton(firstButtonRow, "TryPeek", G_CALLBACK(OnTryPeekClicked));
    addButton(firstButtonRow, "Map (^2)", G_CALLBACK(OnMapClicked));
    addButton(firstButtonRow, "Where (even)", G_CALLBACK(OnWhereClicked));

    addButton(secondButtonRow, "Reduce (sum)", G_CALLBACK(OnReduceClicked));
    addButton(secondButtonRow, "Concat [9,10]", G_CALLBACK(OnConcatClicked));
    addButton(secondButtonRow, "SubStack", G_CALLBACK(OnSubStackClicked));
    addButton(secondButtonRow, "FindSubStack", G_CALLBACK(OnFindSubStackClicked));

    gtk_box_pack_start(GTK_BOX(rootBox), inputRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), firstButtonRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), secondButtonRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(rootBox), outputFrame, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), rootBox);
    gtk_widget_show_all(window);

    ShowUiState(context, "Интерфейс готов");
    gtk_main();

    delete context;
    return 0;
}
