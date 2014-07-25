#include <pebble.h>

#define ODSTEP 38
int GODSTEP=5; //ze wzgledu na godziny musi byc ustawiony w pozycji jak dla wyswietlania dwuelementowej godziny

bool czygodz=true;
bool gnot=true;

Window * window;
TextLayer * godziny1;
TextLayer * godziny2;
TextLayer * minuty1;
TextLayer * minuty2;
TextLayer * data;

const char g[21][15]= {"zerowa","pierwsza","droga","trzecia","czwarta","piata","szosta","siodma","osma","dziewiata","dzisiata","jedenasta","dwunasta","trzynasta","czternasta","pietnasta","szesnasta","siedemnasta","osiemnasta","dziewietnasta","dwudziesta"};
const char m[20][15]= {"","jeden","dwa","trzy","cztery","piec","szesc","siedem","osiem","dziewiec","dziesiec","jedenascie","dwanascie","trzynascie","czternascie","pietnascie","szesnascie","siedemnascie","osiemnascie","dziewietnascie"};
const char m2[6][15]= {"","error","dwadziescia","trzydziesci","czterdziesci","piecdziesiat"};
const char dni[7][15]= {"niedziela ","poniedzialek ","wtorek ","sroda ","czwartek ","piatek ","sobota "};

TextLayer * createtlayer(int poz1,int poz2,int poz3,int poz4,Window * windowek)
{
    TextLayer * wskaz=text_layer_create(GRect(poz1,poz2,poz3,poz4));
    text_layer_set_text(wskaz,"pusty");
    text_layer_set_font(wskaz,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_color(wskaz, GColorWhite);
    text_layer_set_background_color(wskaz, GColorClear);
    layer_set_clips(text_layer_get_layer(wskaz),true);
    layer_add_child(window_get_root_layer(windowek),text_layer_get_layer(wskaz));
    return wskaz;
}

void resettime(const struct tm *t)
{
    int min=t->tm_min;
    bool czy2=true;
    bool czy4=true;
    if (min<20)
    {
        text_layer_set_text(minuty1,m[min]);
        czy4=false;
        text_layer_set_text(minuty2,"");
    }
    else
    {
        int min1=min/10;
        int min2=min%10;
        text_layer_set_text(minuty1,m2[min1]);
        if (min2==0)
            czy4=false;
        text_layer_set_text(minuty2,m[min2]);
    }
    int hour=t->tm_hour;
    if (hour<=20)
    {
        text_layer_set_text(godziny1,g[hour]);
        czy2=false;
        text_layer_set_text(godziny2,"");
    }
    else
    {
        text_layer_set_text(godziny1,g[20]);
        text_layer_set_text(godziny2,g[hour-20]);
    }
    if(czy2)
        GODSTEP=5;
    else
        GODSTEP=15;
    layer_set_frame(text_layer_get_layer(godziny1),GRect(0,GODSTEP,144,ODSTEP));
    if (czy2)
    {
        layer_set_frame(text_layer_get_layer(minuty1),GRect(0,GODSTEP+2*ODSTEP,144,ODSTEP));
        layer_set_frame(text_layer_get_layer(minuty2),GRect(0,GODSTEP+3*ODSTEP,144,ODSTEP));
    }

    else
    {
        layer_set_frame(text_layer_get_layer(minuty1),GRect(0,GODSTEP+ODSTEP,144,ODSTEP));
        layer_set_frame(text_layer_get_layer(minuty2),GRect(0,GODSTEP+ODSTEP*2,144,ODSTEP));
    }
    if(czy4) {}
    if (hour!=0)
        czygodz=true;
    if ((hour==0&&czygodz)||gnot)
    {
        czygodz=false;
        gnot=false;
        static char dan[35]="";
        char date_text[15];
        strcat(dan,dni[t->tm_wday]);
        strftime(date_text, sizeof(date_text), "%F", t);
        text_layer_set_text(data,strcat(dan,date_text));
    }
}
void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
    resettime(tick_time);
}

void handle_init()
{
    window=window_create();
    window_set_background_color(window, GColorBlack);
    window_stack_push(window, true);
    godziny1=createtlayer(0,GODSTEP,144,ODSTEP,window);
    godziny2=createtlayer(0,GODSTEP+ODSTEP,144,ODSTEP,window);
    minuty1=createtlayer(0,GODSTEP+ODSTEP*2,144,ODSTEP,window);
    minuty2=createtlayer(0,GODSTEP+ODSTEP*3,144,ODSTEP,window);
    data=createtlayer(0,140,150,100,window);
    text_layer_set_font(data,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    tick_timer_service_subscribe(MINUTE_UNIT,handle_tick);
    time_t timek=time(NULL);
    resettime(localtime(&timek));
}

void handle_deinit()
{
    tick_timer_service_unsubscribe();
    text_layer_destroy(godziny1);
    text_layer_destroy(godziny2);
    text_layer_destroy(minuty1);
    text_layer_destroy(minuty2);
    text_layer_destroy(data);
    window_destroy(window);
}

int main(void)
{
    handle_init();
    app_event_loop();
    handle_deinit();
}
