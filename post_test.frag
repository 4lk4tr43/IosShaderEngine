varying lowp vec2 uv_var;

void main()
{
	gl_FragColor = vec4(0, uv_var.x, uv_var.y, 1);
}