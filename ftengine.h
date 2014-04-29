#ifndef FTENGINE_H
#define FTENGINE_H
#include "precompile.h"

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_*  FT_Face;
struct FT_LibraryRec_;
struct FT_FaceRec_;

using FT_Library_Deleter = void (*)(FT_Library);
using FT_Face_Deleter    = void (*)(FT_Face);
using FT_Library_Ptr     = std::unique_ptr<FT_LibraryRec_, FT_Library_Deleter>;
using FT_Face_Ptr        = std::unique_ptr<FT_FaceRec_, FT_Face_Deleter>;
//=============================================================
//using FT_Library_Deleter = std::function<void(FT_Library)>;
//using FT_Face_Deleter    = std::function<void(FT_Face)>;
//typedef std::function<void(FT_Library)> FT_Library_Deleter;
//typedef std::function<void(FT_Face)> FT_Face_Deleter;
//typedef void (*FT_Library_Deleter)(FT_Library);
//typedef void (*FT_Face_Deleter   )(FT_Face);
//=============================================================

enum class FACE_TYPE: int8_t {REGULAR, ITALIC, BOLD, ITALIC_BOLD};
enum class FACE_DECORATION: int8_t {UNDERLINE, STRIKEOUT};

struct FTInitData
{
    std::string normal;
    std::string italic;
    std::string bold;
    std::string bold_italic;
    int8_t size_pt;
};

class ColorTable
{
public:
    explicit ColorTable(QRgb color);
    QVector<QRgb> table() const;
private:
    QVector<QRgb> color_table_;
};

class FTEngine
{
public:
    FTEngine();
    explicit FTEngine(const FTInitData& data);

    FTEngine(FTEngine&& other);
    FTEngine& operator=(FTEngine&& other);

    FTEngine(const FTEngine& ) = delete;
    FTEngine& operator=(const FTEngine&) = delete;

    void set_size(int8_t pt);
    bool valid() const;

    ~FTEngine();
private:
    void move_data(FTEngine& other);
    void set_unicode_charset();  // unicode
    void set_size(FT_Face_Ptr& face, int8_t pt, int dpi_x, int dpi_y);
    void draw_wstring(const std::wstring& str, FACE_TYPE type = FACE_TYPE::REGULAR);
    void draw_glyph(FT_Face_Ptr& face, char32_t ch);

private:
    FT_Library_Ptr ft_library_;
    FT_Face_Ptr ft_normal_face_;
    FT_Face_Ptr ft_italic_face_;
    FT_Face_Ptr ft_bold_face_;
    FT_Face_Ptr ft_bold_italic_face_;
//    std::map<QRgb, ColorTable> color_cache_;
};

#endif // FTENGINE_H
