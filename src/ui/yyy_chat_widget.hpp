#pragma once

#include <FL/Fl_Image.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

namespace YYY {

/*---------------------------------------------------------------------------*/
// The chat widget displays a column of dates, a column of names, and a column
// of messages. It is totally independant of the rest of Kashyyyk, and should
// only depend on FLTK components.
// Note that it also does NOT handle scrolling. This is handled by Kashyyyk,
// and the chat widget will be updated when a scroll happens.
class ChatWidget : public Fl_Widget{
private:
    
    enum EnumClickLocation {
        eNoClick,
        eDateSep,
        eNameSep,
        eLine
    };
    
public:
    ChatWidget(int a_x, int a_y, int a_w, int a_h, const char *text = NULL)
      : Fl_Widget(a_x, a_y, a_w, a_h, text)
      , m_font_height(16u)
      , m_font_descent(4u)
      , m_date_sep_x(40)
      , m_name_sep_x(120)
      , m_click_location((unsigned char)eNoClick)
      , m_num_messages(0)
      , m_owns_messages(NULL)
      , m_messages(NULL)
      , m_users(NULL)
      , m_line_padding(2)
      , m_drag_radius(3)
      , m_font_sizes_dirty(true){
        
        m_font_size = labelsize();
        m_font = labelfont();
    }
    
    virtual ~ChatWidget();
    
    virtual void draw();
    virtual int handle(int e);
    
    virtual void resize(int x, int y, int w, int h);
    
    void textsize(Fl_Fontsize size);
    void textfont(Fl_Font font);
    inline Fl_Font textfont() const { return m_font; }
    inline Fl_Fontsize textsize() const { return m_font_size; }
    
    void setMessage(unsigned i, const char *user, const char *msg);
    void setMessageStatic(unsigned i, const char *user, const char *msg);
    const char *getMessage(unsigned i);
    const char *getUser(unsigned i);
    
    void resize(unsigned i);
    inline void clear() { resize(0); }
    inline unsigned size() { return m_num_messages; }
    inline bool empty() { return m_num_messages == 0; }
    
    inline unsigned maxLines() const { return 1 + (h() / (m_line_padding + m_font_height)); }
    
protected:
    
    inline void setDateSeparator(unsigned short i);
    inline void setNameSeparator(unsigned short i);
    
    inline unsigned short getDateSeparator() const { return m_date_sep_x; }
    inline unsigned short getNameSeparator() const { return m_name_sep_x; }
    
    bool withinDragRadius(unsigned short x, unsigned short element) const;
    
private:
    
    void calculateFontHeight();
    
    unsigned short m_font_height, m_font_descent;
    float m_space_width;
    
    unsigned short m_date_sep_x, m_name_sep_x;
    
    int m_click_x, m_click_y;
    
    // Since each message might be static or owned, we must keep track of which
    // messages we allocated. However if no messages have been allocated, we
    // can leave m_owns_messages as NULL to avoid extra allocations (such as if
    // the caller is only using static messages).
    unsigned m_num_messages;
    bool *m_owns_messages;
    const char **m_messages;
    const char **m_users;
    
    unsigned short m_line_padding;
    unsigned short m_drag_radius;

    Fl_Fontsize m_font_size;
    Fl_Font m_font;
    
    bool m_font_sizes_dirty;
    
    unsigned char m_click_location;
    inline EnumClickLocation clickLocation() const { return (EnumClickLocation)m_click_location; }
    inline void clickLocation(EnumClickLocation l) { m_click_location = (unsigned char)l; }
};

} // namespace YYY
