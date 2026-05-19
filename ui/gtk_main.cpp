#include <gtk/gtk.h>
#include <sstream>
#include "Stack.hpp"

struct UiCtx { Stack<int> stack; GtkWidget* entryValue; GtkWidget* entryA; GtkWidget* entryB; GtkWidget* output; };

static int read_int(GtkWidget* e, bool& ok){ ok=false; const char* t=gtk_entry_get_text(GTK_ENTRY(e)); char* end=nullptr; long v=strtol(t?t:"",&end,10); if(end!=t && *end=='\0'){ok=true; return (int)v;} return 0; }
static std::string to_s(const Stack<int>& s){ std::ostringstream o; o<<"["; for(int i=0;i<s.GetSize();++i){ o<<s.Raw().Get(i); if(i+1<s.GetSize()) o<<", "; } o<<"]"; return o.str(); }
static void show(UiCtx* c, const std::string& msg){ std::ostringstream o; o<<msg<<"\nStack="<<to_s(c->stack)<<" size="<<c->stack.GetSize(); gtk_label_set_text(GTK_LABEL(c->output), o.str().c_str()); }

static bool even(const int& x){return x%2==0;} static int sq(const int& x){return x*x;} static int sumf(const int&a,const int&b){return a+b;}

static void on_push(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; bool ok; int v=read_int(c->entryValue,ok); if(!ok) return show(c,"Ошибка value"); c->stack.Push(v); show(c,"Push"); }
static void on_pop(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; auto r=c->stack.TryPop(); show(c, r.IsSome()?"TryPop ok":"TryPop none" ); }
static void on_peek(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; auto r=c->stack.TryPeek(); show(c, r.IsSome()?"TryPeek ok":"TryPeek none" ); }
static void on_map(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; auto m=c->stack.Map<int>(sq); show(c,"Map^2 => "+to_s(m)); }
static void on_where(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; auto w=c->stack.Where(even); show(c,"Where even => "+to_s(w)); }
static void on_reduce(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; show(c,"Reduce sum => "+std::to_string(c->stack.Reduce(sumf,0))); }
static void on_concat(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; int a[2]={9,10}; Stack<int> o(a,2); c->stack=c->stack.Concat(o); show(c,"Concat [9,10]"); }
static void on_sub(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; bool oka,okb; int a=read_int(c->entryA,oka), b=read_int(c->entryB,okb); if(!oka||!okb) return show(c,"Ошибка A/B"); try{ auto s=c->stack.GetSubStack(a,b); show(c,"Substack => "+to_s(s)); }catch(const std::exception&e){ show(c,e.what()); } }
static void on_find(GtkWidget*, gpointer d){ auto*c=(UiCtx*)d; bool oka,okb; int a=read_int(c->entryA,oka), b=read_int(c->entryB,okb); if(!oka||!okb) return show(c,"Ошибка A/B"); int p[2]={a,b}; Stack<int> patt(p,2); show(c,"Find => "+std::to_string(c->stack.FindSubStack(patt))); }

int main(int argc,char**argv){ gtk_init(&argc,&argv); GtkWidget*w=gtk_window_new(GTK_WINDOW_TOPLEVEL); gtk_window_set_title(GTK_WINDOW(w),"LR3 Stack GTK"); gtk_window_set_default_size(GTK_WINDOW(w),1000,520); g_signal_connect(w,"destroy",G_CALLBACK(gtk_main_quit),NULL);
GtkWidget*root=gtk_box_new(GTK_ORIENTATION_VERTICAL,8); gtk_container_set_border_width(GTK_CONTAINER(root),10);
GtkWidget*form=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6); GtkWidget*ev=gtk_entry_new(); GtkWidget*ea=gtk_entry_new(); GtkWidget*eb=gtk_entry_new(); gtk_entry_set_placeholder_text(GTK_ENTRY(ev),"Value"); gtk_entry_set_placeholder_text(GTK_ENTRY(ea),"A/start"); gtk_entry_set_placeholder_text(GTK_ENTRY(eb),"B/end"); gtk_box_pack_start(GTK_BOX(form),ev,FALSE,FALSE,0); gtk_box_pack_start(GTK_BOX(form),ea,FALSE,FALSE,0); gtk_box_pack_start(GTK_BOX(form),eb,FALSE,FALSE,0);
GtkWidget*r1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6), *r2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,6);
GtkWidget*out=gtk_label_new("-"); gtk_label_set_xalign(GTK_LABEL(out),0.0); gtk_label_set_line_wrap(GTK_LABEL(out),TRUE);
UiCtx*c=new UiCtx{Stack<int>(),ev,ea,eb,out};
auto add=[&](GtkWidget*r,const char*t,GCallback cb){ GtkWidget*b=gtk_button_new_with_label(t); g_signal_connect(b,"clicked",cb,c); gtk_box_pack_start(GTK_BOX(r),b,TRUE,TRUE,0);} ;
add(r1,"Push",G_CALLBACK(on_push)); add(r1,"TryPop",G_CALLBACK(on_pop)); add(r1,"TryPeek",G_CALLBACK(on_peek)); add(r1,"Map",G_CALLBACK(on_map)); add(r1,"Where",G_CALLBACK(on_where));
add(r2,"Reduce",G_CALLBACK(on_reduce)); add(r2,"Concat",G_CALLBACK(on_concat)); add(r2,"SubStack",G_CALLBACK(on_sub)); add(r2,"FindSub",G_CALLBACK(on_find));
GtkWidget*fr=gtk_frame_new("Result"); gtk_container_add(GTK_CONTAINER(fr),out);
gtk_box_pack_start(GTK_BOX(root),form,FALSE,FALSE,0); gtk_box_pack_start(GTK_BOX(root),r1,FALSE,FALSE,0); gtk_box_pack_start(GTK_BOX(root),r2,FALSE,FALSE,0); gtk_box_pack_start(GTK_BOX(root),fr,TRUE,TRUE,0);
gtk_container_add(GTK_CONTAINER(w),root); gtk_widget_show_all(w); show(c,"Готово"); gtk_main(); delete c; return 0; }
