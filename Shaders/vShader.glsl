#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute highp vec4 vertex;
attribute highp vec2 uv;

uniform mediump mat4 MVP;

varying highp vec2 uvOut;

void main()
{
    gl_Position = MVP * vertex;
    uvOut = uv;
}
