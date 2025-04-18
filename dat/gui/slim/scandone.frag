#include "lib/sdf.glsl"
#include "lib/math.glsl"

const vec2 dimensions = vec2(24.0);

vec4 effect( vec4 colour, Image tex, vec2 texture_coords, vec2 screen_coords )
{
   vec2 uv = texture_coords*2.0-1.0;
   float m = 1.0 / dimensions.x;

   float d;

   uv = vec2( abs(uv.x), -uv.y );
   d = sdCircle( uv, 0.65 );
   d = max( d, -sdCircle( uv, 0.55 ) );
   d = min( d, sdTriangleEquilateral( (uv + 0.75*vec2(sin(0.0),-cos(0.0))) * -4.0 )-0.1 );
   d = min( d, sdTriangleEquilateral( (uv + 0.75*vec2(sin(-M_PI*2/3),-cos(-M_PI*2/3))) * -4.0 )-0.1 );
   d = min( d, sdCircle( uv, 0.2 ) );

   colour.a = smoothstep( -m, 0.0, -d );

   return colour;
}
