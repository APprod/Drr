#version 300 es

precision mediump float;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

uniform float vignetteIntensity; // 0..1
uniform float vignetteRoundness; // 0 = square, 1 = circle
uniform float vignetteSoftness;  // 0..1

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    // Aspect ratio of the texture
    vec2 texSize = vec2(textureSize(texture0, 0));
    float aspect = texSize.x / texSize.y;

    // Center UV and compensate for aspect ratio
    vec2 uv = fragTexCoord - vec2(0.5);
    uv.x *= aspect;

    // Square <-> Circle distance
    float squareDist = max(abs(uv.x), abs(uv.y));
    float circleDist = length(uv);
    float dist = mix(squareDist, circleDist, vignetteRoundness);

    // Normalize so the corners are always at 1.0
    float maxDist = mix(
        max(aspect, 1.0) * 0.5,
        length(vec2(aspect, 1.0)) * 0.5,
        vignetteRoundness
    );

    dist /= maxDist;

    // Vignette mask
    float vignette = smoothstep(
        1.0 - vignetteSoftness,
        1.0,
        clamp(dist, 0.0, 1.0)
    );

    color.rgb *= 1.0 - vignette * vignetteIntensity;

    finalColor = color * fragColor;
}