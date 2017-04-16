#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying highp vec2 uvOut;
uniform sampler2D uvUniform;

void main()
{
        gl_FragColor = texture2D( uvUniform, uvOut );
}
