#include <gtk/gtk.h>
#include <sstream>
#include <string>
#include "ArraySequence.hpp"
#include "ListSequence.hpp"
#include "BitSequence.hpp"

struct UiCtx {
    ArraySequence<int> arraySequence;
    ListSequence<int> listSequence;
    BitSequence bitSequence;

    GtkWidget* entryValue;
    GtkWidget* entryIndex;
    GtkWidget* output;
};

static bool IsEven(const int& value) { return value % 2 == 0; }
static bool IsZero(const int& value) { return value == 0; }

static int read_int(GtkWidget* entry, bool& ok) {
    ok = false;
    const char* text = gtk_entry_get_text(GTK_ENTRY(entry));
    char* end = nullptr;
    long value = strtol(text ? text : "", &end, 10);
    if (end != text && *end == '\0') {
        ok = true;
        return static_cast<int>(value);
    }
    return 0;
}

static std::string int_seq_to_string(Sequence<int>& seq) {
    std::ostringstream os;
    os << "[";
    for (int i = 0; i < seq.GetSize(); ++i) {
        os << seq.Get(i);
        if (i + 1 < seq.GetSize()) os << ", ";
    }
    os << "]";
    return os.str();
}

static std::string bit_seq_to_string(BitSequence& seq) {
    std::ostringstream os;
    for (int i = 0; i < seq.GetSize(); ++i) os << (seq.Get(i).Get() ? 1 : 0);
    return os.str();
}

static void refresh(UiCtx* ctx, const std::string& message) {
    std::ostringstream os;
    os << message << "\n\n"
       << "ArraySequence: " << int_seq_to_string(ctx->arraySequence) << "\n"
       << "ListSequence: " << int_seq_to_string(ctx->listSequence) << "\n"
       << "BitSequence: " << bit_seq_to_string(ctx->bitSequence) << "\n";
    gtk_label_set_text(GTK_LABEL(ctx->output), os.str().c_str());
}

static void on_append_array(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool ok; int v=read_int(c->entryValue,ok); if(!ok) return refresh(c,"Ошибка Value"); c->arraySequence.Append(v); refresh(c,"Append Array"); }
static void on_append_list(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool ok; int v=read_int(c->entryValue,ok); if(!ok) return refresh(c,"Ошибка Value"); c->listSequence.Append(v); refresh(c,"Append List"); }
static void on_prepend_array(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool ok; int v=read_int(c->entryValue,ok); if(!ok) return refresh(c,"Ошибка Value"); c->arraySequence.Prepend(v); refresh(c,"Prepend Array"); }
static void on_prepend_list(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool ok; int v=read_int(c->entryValue,ok); if(!ok) return refresh(c,"Ошибка Value"); c->listSequence.Prepend(v); refresh(c,"Prepend List"); }

static void on_insert_array(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool okV,okI; int v=read_int(c->entryValue,okV), i=read_int(c->entryIndex,okI); if(!okV||!okI) return refresh(c,"Ошибка Value/Index"); try{ c->arraySequence.InsertAt(i,v); refresh(c,"InsertAt Array"); }catch(const std::exception& e){ refresh(c,e.what()); }}
static void on_insert_list(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool okV,okI; int v=read_int(c->entryValue,okV), i=read_int(c->entryIndex,okI); if(!okV||!okI) return refresh(c,"Ошибка Value/Index"); try{ c->listSequence.InsertAt(i,v); refresh(c,"InsertAt List"); }catch(const std::exception& e){ refresh(c,e.what()); }}
static void on_set_array(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool okV,okI; int v=read_int(c->entryValue,okV), i=read_int(c->entryIndex,okI); if(!okV||!okI) return refresh(c,"Ошибка Value/Index"); try{ c->arraySequence.Set(i,v); refresh(c,"Set Array"); }catch(const std::exception& e){ refresh(c,e.what()); }}
static void on_set_list(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool okV,okI; int v=read_int(c->entryValue,okV), i=read_int(c->entryIndex,okI); if(!okV||!okI) return refresh(c,"Ошибка Value/Index"); try{ c->listSequence.Set(i,v); refresh(c,"Set List"); }catch(const std::exception& e){ refresh(c,e.what()); }}

static void on_filter_even(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; auto ev=c->arraySequence.Filter(IsEven); refresh(c,"Filter even => "+int_seq_to_string(ev)); }
static void on_map_square(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; auto sq=c->arraySequence.Map<int>([](int x){return x*x;}); refresh(c,"Map square => "+int_seq_to_string(sq)); }
static void on_reduce_sum(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; int s=c->arraySequence.Reduce<int>(0,[](const int& a,const int& b){return a+b;}); refresh(c,"Reduce sum => "+std::to_string(s)); }
static void on_split_zero(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; auto parts=c->arraySequence.Split(IsZero); refresh(c,"Split by 0 => parts="+std::to_string(parts.GetSize())); }
static void on_slice_demo(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; int raw[2]={9,10}; ArraySequence<int> ins(raw,2); try{ c->arraySequence.Slice(1,2,&ins); refresh(c,"Slice(1,2,[9,10]) done"); }catch(const std::exception& e){ refresh(c,e.what()); }}
static void on_zip_demo(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; int n=c->arraySequence.GetSize(); ArraySequence<int> other; for(int i=0;i<n;++i) other.Append((i+1)*10); auto z=c->arraySequence.Zip(other); std::ostringstream os; os<<"Zip with [10,20,...] size="<<z.GetSize(); refresh(c,os.str()); }

static void on_bit_add(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; bool ok; int b=read_int(c->entryValue,ok); if(!ok||(b!=0&&b!=1)) return refresh(c,"Bit only 0/1"); c->bitSequence.Append(Bit(b==1)); refresh(c,"Bit append"); }
static void on_bit_not(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; c->bitSequence = c->bitSequence.Not(); refresh(c,"Bit NOT"); }
static void on_show(GtkWidget*, gpointer d){ UiCtx* c=(UiCtx*)d; refresh(c,"Текущее состояние"); }

int main(int argc, char** argv){
    gtk_init(&argc,&argv);
    GtkWidget* win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win),"LR2 GTK ручное тестирование");
    gtk_window_set_default_size(GTK_WINDOW(win),1100,700);
    g_signal_connect(win,"destroy",G_CALLBACK(gtk_main_quit),NULL);

    GtkWidget* root=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_container_set_border_width(GTK_CONTAINER(root),10);

    GtkWidget* form=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    GtkWidget* ev=gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(ev),"Value");
    GtkWidget* ei=gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(ei),"Index");
    gtk_box_pack_start(GTK_BOX(form), gtk_label_new("Value:"), FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(form), ev, FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(form), gtk_label_new("Index:"), FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(form), ei, FALSE,FALSE,0);

    GtkWidget* row1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    GtkWidget* row2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
    GtkWidget* row3=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);

    auto btn=[&](GtkWidget* row,const char* txt,GCallback cb,UiCtx* c){ GtkWidget* b=gtk_button_new_with_label(txt); g_signal_connect(b,"clicked",cb,c); gtk_box_pack_start(GTK_BOX(row),b,TRUE,TRUE,0); };

    GtkWidget* out=gtk_label_new("-"); gtk_label_set_xalign(GTK_LABEL(out),0.0); gtk_label_set_line_wrap(GTK_LABEL(out),TRUE); gtk_label_set_selectable(GTK_LABEL(out),TRUE);
    GtkWidget* frame=gtk_frame_new("Результат"); gtk_container_add(GTK_CONTAINER(frame),out);

    UiCtx* c=new UiCtx{ArraySequence<int>(),ListSequence<int>(),BitSequence(),ev,ei,out};

    btn(row1,"Append Array",G_CALLBACK(on_append_array),c); btn(row1,"Append List",G_CALLBACK(on_append_list),c); btn(row1,"Prepend Array",G_CALLBACK(on_prepend_array),c); btn(row1,"Prepend List",G_CALLBACK(on_prepend_list),c);
    btn(row1,"InsertAt Array",G_CALLBACK(on_insert_array),c); btn(row1,"InsertAt List",G_CALLBACK(on_insert_list),c);

    btn(row2,"Set Array",G_CALLBACK(on_set_array),c); btn(row2,"Set List",G_CALLBACK(on_set_list),c); btn(row2,"Filter even",G_CALLBACK(on_filter_even),c); btn(row2,"Map square",G_CALLBACK(on_map_square),c); btn(row2,"Reduce sum",G_CALLBACK(on_reduce_sum),c);
    btn(row2,"Split by 0",G_CALLBACK(on_split_zero),c); btn(row2,"Slice demo",G_CALLBACK(on_slice_demo),c); btn(row2,"Zip demo",G_CALLBACK(on_zip_demo),c);

    btn(row3,"Bit append(0/1)",G_CALLBACK(on_bit_add),c); btn(row3,"Bit NOT",G_CALLBACK(on_bit_not),c); btn(row3,"Refresh",G_CALLBACK(on_show),c);

    gtk_box_pack_start(GTK_BOX(root), form, FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(root), row1, FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(root), row2, FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(root), row3, FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(root), frame, TRUE,TRUE,0);

    gtk_container_add(GTK_CONTAINER(win),root);
    gtk_widget_show_all(win);
    refresh(c,"Интерфейс готов");
    gtk_main();
    delete c;
    return 0;
}
