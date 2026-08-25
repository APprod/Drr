#include "rendering/textureSpec.hpp"
#include "services.hpp"
#include "utils/util.hpp"

TextureSpec::TextureSpec(const std::string& textureName)
    : texture(std::move(textureName))
    , slice(GetServices().resManager.getSliceData(texture))
{}

void TextureSpec::Draw(Rectangle dest) const {
    auto tex = GetServices().resManager.getTexture(texture);

    auto drawCall = [&, this]() {
        if (slice) {
            auto& s = *slice;
            NPatchInfo npi = {
                rect(tex),
                s.left, s.top, s.right, s.bottom,
                NPATCH_NINE_PATCH
            };
            DrawTextureNPatch(tex, npi, dest, {0, 0}, 0, tint);
        } else {
            DrawTexturePro(tex, rect(tex), dest, {0, 0}, 0, tint);
        }
    };

    if (processing) {
        auto& p = *processing;
        useShaderUnchecked(p.programName, p.uniforms, drawCall);
    } else {
        drawCall();
    }
}
