#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

uniform float brightness; // 1.0 = normal
uniform float contrast;   // 1.0 = normal
uniform float saturation; // 1.0 = normal
uniform float gamma;      // 1.0 = normal

uniform vec3 tint;         // (1,1,1) = normal
uniform float alpha;       // 1.0 = normal

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    // Brightness
    color.rgb *= brightness;

    // Contrast
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;

    // Saturation
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    color.rgb = mix(vec3(gray), color.rgb, saturation);

    // Tint
    color.rgb *= tint;

    // Gamma correction
    color.rgb = pow(max(color.rgb, vec3(0.0)), vec3(1.0 / gamma));

    // Alpha
    color.a *= alpha;

    finalColor = color * fragColor;
}