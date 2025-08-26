#include <windows.h>       // Para HBITMAP, BITMAPINFO, CreateDIBSection, etc.
#include <fstream>
#include <vector>
#include <string>          // Por std::string

#pragma pack(push, 1)
struct RTXHeader {
    uint32_t width;
    uint32_t height;
    uint32_t format; // 4 = RGBA8888
};
#pragma pack(pop)

HBITMAP cargarRTXComoBitmap(const std::string& rutaRTX) {
    std::ifstream archivo(rutaRTX, std::ios::binary);
    if (!archivo) return NULL;

    RTXHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.format != 4) return NULL;

    std::vector<uint8_t> pixels(header.width * header.height * 4);
    archivo.read(reinterpret_cast<char*>(pixels.data()), pixels.size());

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = header.width;
    bmi.bmiHeader.biHeight = -static_cast<int>(header.height); // Imagen "al derecho"
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!hBitmap) return NULL;

    // Convertir RGBA a BGRA (Windows usa BGRA)
    uint8_t* dst = static_cast<uint8_t*>(bits);
    for (size_t i = 0; i < header.width * header.height; ++i) {
        dst[i * 4 + 0] = pixels[i * 4 + 2]; // Blue
        dst[i * 4 + 1] = pixels[i * 4 + 1]; // Green
        dst[i * 4 + 2] = pixels[i * 4 + 0]; // Red
        dst[i * 4 + 3] = pixels[i * 4 + 3]; // Alpha
    }

    return hBitmap;
}
