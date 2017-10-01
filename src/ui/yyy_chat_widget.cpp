#define NOMINMAX 1
#include "yyy_chat_widget.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.h>

#include <algorithm> // For std::max

#include <cassert>

#if (defined __WATCOMC__) || (defined _MSC_VER)
#include <malloc.h>
#elif defined __GNUC__
#include <stdlib.h>
#else
#include <alloca.h>
#endif

namespace YYY {

/*===========================================================================*/

struct ClippingRectangle {
    ClippingRectangle(int x, int y, unsigned w, unsigned h){
        fl_push_clip(x, y, w, h);
    }
    ~ClippingRectangle(){
        fl_pop_clip();
    }
};

/*---------------------------------------------------------------------------*/

static bool s_mouse_resizing = false;

/*---------------------------------------------------------------------------*/

static void should_be_resizing(Fl_Window &window){
    if(!s_mouse_resizing){
        s_mouse_resizing = true;
        window.cursor(FL_CURSOR_WE);
    }
}

/*---------------------------------------------------------------------------*/

static void should_not_be_resizing(Fl_Window &window){
    if(s_mouse_resizing){
        s_mouse_resizing = false;
        window.cursor(FL_CURSOR_DEFAULT);
    }
}

/*---------------------------------------------------------------------------*/

static inline bool yyy_is_alpha(const char c){
    return (c & 0x80) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*---------------------------------------------------------------------------*/

static inline bool yyy_is_space(const char c){
    return c < 32 || c == ' ';
}

/*---------------------------------------------------------------------------*/

static unsigned yyy_get_word_width(const char *word, unsigned &in_out_index,
    unsigned max_len){
    
    unsigned i = in_out_index;
    while(i < max_len && yyy_is_alpha(word[i]))
        i++;
    
    while(i < max_len && !(yyy_is_alpha(word[i]) || yyy_is_space(word[i])))
        i++;
    
    if(i == in_out_index)
        return 0;
    const unsigned w = fl_width(word + in_out_index, i - in_out_index);
    in_out_index = i;
    return w;
}

/*---------------------------------------------------------------------------*/

static unsigned yyy_get_whitespace_width(const char *word,
    unsigned &in_out_index, const unsigned max_len, const float space_width){
    
    unsigned i = in_out_index;
    while(i < max_len && yyy_is_space(word[i]))
        i++;
    
    if(i == in_out_index)
        return 0;
    
    const unsigned w = space_width * static_cast<float>(i - in_out_index);
    in_out_index = i;
    return w;
}

/*---------------------------------------------------------------------------*/

static inline unsigned short yyy_expand_text(unsigned short lines[][2],
    const unsigned max_lines,
    const unsigned max_w,
    const float space_width,
    const char *const in_message,
    char *out_message = NULL,
    unsigned message_buffer_len = 0){
    
    if(*in_message == '\0'){
        lines[0][0] = 0;
        lines[0][1] = 0;
        if(message_buffer_len)
            out_message[0] = '\0';
        return 0;
    }
    
    unsigned msg_len = strlen(in_message);
    
    if(message_buffer_len != 0){
        assert(msg_len < message_buffer_len);
        if(msg_len >= message_buffer_len)
            msg_len = message_buffer_len - 1;
    }
    
    unsigned short line_width = 0, line_num = 0;
    lines[0][0] = 0;
    unsigned i = 0;
    while(i < msg_len && line_num + 1u < max_lines){
        const char c = in_message[i];
        const unsigned start = i;
        if(yyy_is_space(c)){
            const unsigned w = yyy_get_whitespace_width(
                in_message, i, msg_len, space_width);
            
            if(message_buffer_len)
                for(unsigned e = start; e < i; e++)
                    out_message[e] = ' ';
            if(w + line_width > max_w){
                lines[line_num][1] = start - lines[line_num][0];
                lines[++line_num][0] = i;
                line_width = 0;
            }
            else
                line_width += w;
        }
        else{
            unsigned w;
            if(yyy_is_alpha(c)){
                w = yyy_get_word_width(in_message, i, msg_len);
                const unsigned word_len = i - start;
                if(message_buffer_len)
                    memcpy(out_message + start, in_message + start, word_len);
            }
            else{
                w = fl_width(&c, 1);
                if(message_buffer_len)
                    out_message[i++] = c;
            }
            if(w + line_width > max_w){
                lines[line_num][1] = start - lines[line_num][0];
                lines[++line_num][0] = start;
                line_width = w;
            }
            else
                line_width += w;
        }
    }
    
    lines[line_num][1] = i - lines[line_num][0];
    
    return line_num;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::draw(){
    if(m_font_sizes_dirty){
        m_font_sizes_dirty = false;
        calculateFontHeight();
    }
    draw_box();
    
    const Fl_Boxtype boxtype = box();
    
    const int x_ = x(), y_ = y(), w_ = w(), h_ = h(), 
        box_x = Fl::box_dx(boxtype), box_y = Fl::box_dy(boxtype),
        box_w = Fl::box_dw(boxtype), box_h = Fl::box_dh(boxtype),
        max_w = w_ - m_name_sep_x - 4 - box_w;
    
    (void)box_x;
    
    ClippingRectangle widget_rect = ClippingRectangle(x_, y_, w_, h_);
    fl_color(fl_lighter(FL_FOREGROUND_COLOR));
    fl_font(m_font, m_font_size);
    const unsigned line_height = m_line_padding + m_font_height,
        line_x = x_ + m_name_sep_x + 2;
    int offset = h_ - fl_descent() - m_line_padding;
    
    unsigned short lines[312][2];
    char message[256];
    
    unsigned name_i = 0;
    
    // Used to cache name widths for statically stored names.
    const char *last_user[4] = { NULL, NULL, NULL, NULL };
    unsigned short name_width[4];
    
    for(unsigned msg_num = m_num_messages; msg_num && offset; msg_num--){
        const unsigned max_lines = 312;
        unsigned short line_num = yyy_expand_text(lines,
            max_lines,
            max_w,
            m_space_width,
            m_messages[msg_num - 1],
            message,
            sizeof(message));
        
        offset -= line_height * (line_num + 1);
        for(unsigned e = line_num + 1; e != 0; e--){
            const unsigned i = line_num - (e-1);
//            const unsigned line_len = lines[i + 1] - lines[i];
            fl_draw(message + lines[i][0], lines[i][1], line_x,
                y_ + offset + (line_height * (line_num - e + 2)));
        }
        
        {
            ClippingRectangle name_box_rect = ClippingRectangle(
                x_ + m_date_sep_x, y_, m_name_sep_x - m_date_sep_x, h_);
            const char *const user = m_users[msg_num - 1];
            unsigned short l_name_width = 0;
            for(unsigned e = 0; e < 4; e++){
                if(last_user[e] == user){
                    printf("Cached on %s/%i\n", user, e);
                    l_name_width = name_width[e];
                    name_i = ++e & 3;
                    break;
                }
            }
            
            if(l_name_width == 0){ // Avoid unneeded measurements for reused names.
                name_i++;
                name_i &= 3;
                name_width[name_i] = l_name_width = fl_width(user);
                last_user[name_i] = user;
            }
            fl_draw(user, x_ + m_name_sep_x - l_name_width - 2, y_ + offset + line_height);
        }
    }
    
    {
        // Draw divider lines
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        const unsigned top_y = y_ + box_y;
        fl_yxline(x_ + m_date_sep_x, top_y, top_y + h_ - box_h);
        fl_yxline(x_ + m_name_sep_x, top_y, top_y + h_ - box_h);
    }
}

/*---------------------------------------------------------------------------*/

int ChatWidget::handle(int e){
    const int x_ = x(), y_ = y(), w_ = w(), h_ = h();
    switch(e){
        case FL_ENTER:
            // Ask for movement events.
            return 1;
            
        case FL_MOVE:
            {
                const int ex = Fl::event_x();
                if(withinDragRadius(ex, m_date_sep_x) || withinDragRadius(ex, m_name_sep_x))
                    should_be_resizing(*window());
                else
                    should_not_be_resizing(*window());
            }
            return 1;
            
        case FL_LEAVE:
            should_not_be_resizing(*window());
            return 1;
            
        case FL_PUSH:
            if(Fl::event_button() & FL_LEFT_MOUSE){
                m_click_x = Fl::event_x();
                m_click_y = Fl::event_y();
                if(m_click_x < x_ || m_click_y < y_ ||
                    m_click_x > x_ + w_ || m_click_y > y_ + h_){
                    m_click_location = eNoClick;
                    return 0;
                }
                if(withinDragRadius(m_click_x, m_date_sep_x))
                    m_click_location = eDateSep;
                else if(withinDragRadius(m_click_x, m_name_sep_x))
                    m_click_location = eNameSep;
            }
            else
                m_click_location = eNoClick;
            return 1;
            
        case FL_RELEASE:
            m_click_location = eNoClick;
            return 1;
        
        case FL_DRAG:
            {
                const int click_x = Fl::event_x(),
                    click_y = Fl::event_y(),
                    x_ = x(),
                    w_ = w(),
                    dest = x_ < 0 ? 0 : click_x < x_ ? 0 : click_x - x_;
                    
                (void)click_y;
                (void)w_;
                
                switch(m_click_location){
                    case eDateSep:
                        setDateSeparator(dest);
                        break;
                    case eNameSep:
                        setNameSeparator(dest);
                        break;
                    default: break;
                }
                
                if(m_click_location == eDateSep || m_click_location == eNameSep){
                    redraw();
                    should_be_resizing(*window());
                }
                else
                    should_not_be_resizing(*window());
                
                return 1;
            }
    }
    
    return Fl_Widget::handle(e);
}

/*---------------------------------------------------------------------------*/

void ChatWidget::resize(int x, int y, int w, int h){
    Fl_Widget::resize(x, y, w, h);
    
    if(m_name_sep_x < 16)
        m_name_sep_x = 16;
        
    if(m_date_sep_x < 8)
        m_date_sep_x = 8;
    
    setDateSeparator(m_date_sep_x);
    setNameSeparator(m_name_sep_x);
}

/*---------------------------------------------------------------------------*/

ChatWidget::~ChatWidget(){
    clear();
    if(m_owns_messages)
        free(m_owns_messages);
    if(m_messages){
        free(m_messages);
        free(m_users);
    }
}

/*---------------------------------------------------------------------------*/

void ChatWidget::textsize(Fl_Fontsize size) {
    m_font_sizes_dirty = true;
    m_font_size = size;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::textfont(Fl_Font font) {
    m_font_sizes_dirty = true;
    m_font = font;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::setMessage(unsigned i, const char *usr, const char *msg){
    if(m_num_messages <= i)
        return;
    
    if(usr == NULL){
        setMessage(i, "", msg);
        return;
    }
    
    if(msg == NULL){
        setMessage(i, usr, "");
        return;
    }
    
    if(*msg == '\0' && *usr == '\0'){
        setMessageStatic(i, "", "");
    }
    
    if(m_owns_messages == NULL)
        m_owns_messages = (bool*)calloc(1, m_num_messages);
    
    const unsigned msg_len = strlen(msg),
        usr_len = strlen(usr);
    char *msg_buffer, *usr_buffer;
    if(m_owns_messages[i]){
        msg_buffer = (char*)realloc((char*)m_messages[i], msg_len + 1);
        usr_buffer = (char*)realloc((char*)m_users[i], usr_len + 1);
        
    }
    else{
        msg_buffer = (char*)malloc(msg_len + 1);
        usr_buffer = (char*)malloc(usr_len + 1);
    }
    memcpy(msg_buffer, msg, msg_len+1);
    memcpy(usr_buffer, usr, usr_len+1);

    m_messages[i] = msg_buffer;
    m_users[i] = usr_buffer;
    m_owns_messages[i] = true;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::setMessageStatic(unsigned i, const char *usr, const char *msg){
    if(m_num_messages <= i)
        return;
    
    if(usr == NULL){
        setMessageStatic(i, "", msg);
        return;
    }
    
    if(msg == NULL){
        setMessageStatic(i, usr, "");
        return;
    }
    
    if(m_owns_messages != NULL && m_owns_messages[i]){
        free((void*)m_messages[i]);
        free((void*)m_users[i]);
        m_owns_messages[i] = false;
    }
    
    m_messages[i] = msg;
    m_users[i] = usr;
}

/*---------------------------------------------------------------------------*/

const char *ChatWidget::getMessage(unsigned i){
    if(m_num_messages > i)
        return m_messages[i];
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

const char *ChatWidget::getUser(unsigned i){
    if(m_num_messages > i)
        return m_users[i];
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::resize(unsigned len){
    if(m_owns_messages != NULL){
        for(unsigned i = len; i < m_num_messages; i++){
            if(m_owns_messages[i])
                free((void*)m_messages[i]);
            m_messages[i] = NULL;
            m_users[i] = NULL;
        }
    }
    if(len > m_num_messages){
        m_messages = (const char**)realloc(m_messages, sizeof(char*)*len);
        m_users = (const char**)realloc(m_users, sizeof(char*)*len);
        for(unsigned i = m_num_messages; i < len; i++){
            m_messages[i] = "";
            m_users[i] = "";
        }
        if(m_owns_messages != NULL){
            m_owns_messages = (bool*)realloc(m_owns_messages, sizeof(bool)*len);
            for(unsigned i = m_num_messages; i < len; i++)
                m_owns_messages[i] = false;
        }
        
    }
    m_num_messages = len;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::setDateSeparator(unsigned short dest){
    if(dest + 8 > m_name_sep_x)
        m_date_sep_x = m_name_sep_x - 8;
    else if(dest < 8)
        m_date_sep_x = 8;
    else{
        const unsigned max = w() - 16u;
        if(dest > max)
            m_date_sep_x = max;
        else
            m_date_sep_x = dest;
    }
}

/*---------------------------------------------------------------------------*/

void ChatWidget::setNameSeparator(unsigned short dest){
    if(dest < m_date_sep_x + 8)
        m_name_sep_x = m_date_sep_x + 8;
    else if(dest < 16)
        m_name_sep_x = 16;
    else{
        const unsigned max = w() - 8u;
        if(dest > max)
            m_name_sep_x = max;
        else
            m_name_sep_x = dest;
    }
}

/*---------------------------------------------------------------------------*/

bool ChatWidget::withinDragRadius(unsigned short x_, unsigned short e) const {
    const int ex = x_ - x();
    return ex + m_drag_radius >= e && ex - m_drag_radius <= e;
}

/*---------------------------------------------------------------------------*/

void ChatWidget::calculateFontHeight(){
    fl_font(m_font, m_font_size);
    m_font_descent = fl_descent();
    m_font_height = fl_height();
    const char c = ' ';
    m_space_width = fl_width(&c, 1);
}

} // namespace YYY

