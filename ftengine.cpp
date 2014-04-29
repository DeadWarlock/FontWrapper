#include "precompile.h"
#include <QApplication>
#include <QDesktopWidget>

#include "ftengine.h"
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library   new_ft_library();
FT_Face      new_ft_face(FT_Library library, const std::string& face_path);
void         delete_ft_library(FT_Library ptr);
void         delete_ft_face(FT_Face ptr);
void         set_charset(FT_Face_Ptr& face, FT_Encoding enc);

FTEngine::FTEngine()
    : ft_library_(nullptr, delete_ft_library)
    , ft_normal_face_(nullptr, delete_ft_face)
    , ft_italic_face_(nullptr, delete_ft_face)
    , ft_bold_face_(nullptr, delete_ft_face)
    , ft_bold_italic_face_(nullptr, delete_ft_face)
{
}

FTEngine::FTEngine(const FTInitData& data)
    : FTEngine()
{
    ft_library_.reset(new_ft_library());
    if (!ft_library_)
        return;
    ft_normal_face_.reset(new_ft_face(ft_library_.get(), data.normal));
    ft_italic_face_.reset(new_ft_face(ft_library_.get(), data.italic));
    ft_bold_face_.reset(new_ft_face(ft_library_.get(), data.bold));
    ft_bold_italic_face_.reset(new_ft_face(ft_library_.get(), data.bold_italic));
    set_unicode_charset();
    set_size(data.size_pt);
}

FTEngine::FTEngine(FTEngine&& other)
    : FTEngine()
{
    move_data(other);
}

FTEngine& FTEngine::operator=(FTEngine&& other)
{
    move_data(other);
    return *this;
}

void FTEngine::set_size(int8_t pt)
{
    if (!valid())
        return;
    QDesktopWidget* desktop = QApplication::desktop();
    int dpi_x, dpi_y;
    dpi_x = dpi_y = 96;
    if (desktop)
    {
        dpi_x = desktop->logicalDpiX();
        dpi_y = desktop->logicalDpiY();
    }
    else
        qDebug() << "FT error: Get DPI fail;";
    set_size(ft_normal_face_,       pt, dpi_x, dpi_y);
    set_size(ft_italic_face_,       pt, dpi_x, dpi_y);
    set_size(ft_bold_face_,         pt, dpi_x, dpi_y);
    set_size(ft_bold_italic_face_,  pt, dpi_x, dpi_y);
}

bool FTEngine::valid() const
{
    const bool valid = ft_library_ &&
                       ft_normal_face_ &&
                       ft_italic_face_ &&
                       ft_bold_face_ &&
                       ft_bold_italic_face_;
    return valid;
}

FTEngine::~FTEngine()
{
    ft_bold_italic_face_.reset();
    ft_bold_face_.reset();
    ft_italic_face_.reset();
    ft_normal_face_.reset();
    ft_library_.reset();
}

void FTEngine::move_data(FTEngine& other)
{
    ft_normal_face_ = std::move(other.ft_normal_face_);
    ft_italic_face_ = std::move(other.ft_italic_face_);
    ft_bold_face_ = std::move(other.ft_bold_face_);
    ft_bold_italic_face_ = std::move(other.ft_bold_italic_face_);
    ft_library_ = std::move(other.ft_library_);
}

void FTEngine::set_unicode_charset()
{
    if (!valid())
        return;
    set_charset(ft_normal_face_,      FT_ENCODING_UNICODE);
    set_charset(ft_italic_face_,      FT_ENCODING_UNICODE);
    set_charset(ft_bold_face_,        FT_ENCODING_UNICODE);
    set_charset(ft_bold_italic_face_, FT_ENCODING_UNICODE);
}



void FTEngine::set_size(FT_Face_Ptr& face, int8_t pt, int dpi_x, int dpi_y)
{
    FT_Error error = FT_Set_Char_Size(face.get(),
                                      0, pt * 64,
                                      dpi_x, dpi_y);
    if (error)
    {
        qDebug() << "FT error: Fase Size fail" << face.get() << error;
        face.reset();
    }
}

void FTEngine::draw_wstring(const std::wstring& str, FACE_TYPE type)
{
    if (!valid())
        return;
    FT_Face_Ptr* face;
    switch (type)
    {
    case FACE_TYPE::REGULAR:
        face = &ft_normal_face_;
        break;
    case FACE_TYPE::ITALIC:
        face = &ft_italic_face_;
        break;
    case FACE_TYPE::BOLD:
        face = &ft_bold_face_;
        break;
    case FACE_TYPE::ITALIC_BOLD:
        face = &ft_bold_italic_face_;
        break;
    }
    for (const auto& ch : str)
        draw_glyph(*face, ch);
}

void FTEngine::draw_glyph(FT_Face_Ptr& face, char32_t ch)
{
    FT_UInt glyph_index = 0;
    glyph_index = FT_Get_Char_Index(face.get(), ch);

    FT_Error error = FT_Load_Glyph(face.get(),
                          glyph_index,
                          FT_LOAD_DEFAULT);
    if (error)
        return;
//    if (!error)
//    {
//        FT_Pos left = m_face->glyph->metrics.horiBearingX;
//        FT_Pos right = left + m_face->glyph->metrics.width;
//        FT_Pos top = m_face->glyph->metrics.horiBearingY;
//        FT_Pos bottom = top - m_face->glyph->metrics.height;

//        m_glyphRect = QRect(QPoint(TRUNC(left),
//                                   -TRUNC(top) + 1),
//                            QSize(TRUNC(right - left) + 1,
//                                  TRUNC(top - bottom) + 1));
//        setFixedSize(m_glyphRect.width(),
//                     m_glyphRect.height());
//    }
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    QImage glyph_image(face->glyph->bitmap.buffer,
                      face->glyph->bitmap.width,
                      face->glyph->bitmap.rows,
                      face->glyph->bitmap.pitch,
                      QImage::Format_Indexed8);
//    color_cache_[qRgb(0, 0, 0)] = ColorTable(qRgb(0, 0, 0));
//    glyph_image.setColorTable(color_cache_[qRgb(0, 0, 0)].table());

}


ColorTable::ColorTable(QRgb color)
    :color_table_(256)
{
    for (int i = 0; i < 256; ++i)
        color_table_[i] = qRgba(qRed(color), qGreen(color), qBlue(color), i);
    color_table_.squeeze();
}

QVector<QRgb> ColorTable::table() const
{
    return color_table_;
}

//==============================================================================
FT_Library new_ft_library()
{
    FT_Library t;
    FT_Error error = FT_Init_FreeType(&t);
    if (error)
    {
        qDebug() << "FT error: Library initialization fail;" << error;
        return nullptr;
    }
    else
        qDebug() << "FT: Library init done" << t;
    return t;
}

void delete_ft_library(FT_Library ptr)
{
    qDebug() << "Try Delete library" << ptr;
    FT_Error error = FT_Done_FreeType(ptr);
    if (error)
        qDebug() << "FT error: Library deletion fail;" << error;
    else
        qDebug() << "Library was deleted;";
}


FT_Face new_ft_face(FT_Library library, const std::string& face_path)
{
    FT_Face t;
    FT_Error error = FT_New_Face(library, face_path.c_str(), 0, &t);
    if (error == FT_Err_Unknown_File_Format)
    {
        qDebug() << "FT error: Face unknown file;";
        return nullptr;
    }
    else
    {
        if (error)
        {
            qDebug() << "FT error: Face initialization fail;" << error;
            return nullptr;
        }
        else
            qDebug() << "FT: Face init done" << t;
    }
    return t;
}

void delete_ft_face(FT_Face ptr)
{
    qDebug() << "Try Delete face" << ptr;
    FT_Error error = FT_Done_Face(ptr);
    if (error)
        qDebug() << "FT error: Face deletion fail;" << error;
    else
        qDebug() << "Face was deleted;";
}

void set_charset(FT_Face_Ptr& face, FT_Encoding enc)
{
    FT_Error error = FT_Select_Charmap(face.get(), enc /*FT_ENCODING_UNICODE*/);
//  FT_Error error = FT_Select_Charmap(res.first->second.face_.get(), FT_ENCODING_NONE);
    if (error)
    {
        qDebug() << "FT error: Select Unicode charmap" << face.get() << error;
        face.reset();
    }
}
