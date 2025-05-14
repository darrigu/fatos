uniform vec2 resolution;
uniform float time;

void main() {
   vec2 uv = gl_FragCoord.xy/resolution.xy;
   vec2 centeredUV = (uv - 0.5)*2.0;
   centeredUV.x *= resolution.x/resolution.y;
   
   vec3 bg = mix(
      vec3(0.03, 0.0, 0.06), 
      vec3(0.1, 0.0, 0.15), 
      uv.y
   );
   
   float wave1 = sin(centeredUV.x*8.0 + time*1.5)*0.02;
   float wave2 = cos(centeredUV.x*6.0 - time*1.2)*0.03;
   float waveY = smoothstep(0.3, 0.7, uv.y + wave1 + wave2);
   vec3 waves = vec3(waveY*0.15, 0.0, waveY*0.25);
   
   float gridSize = 40.0;
   vec2 gridUV = uv*resolution.xy/gridSize;
   float grid = max(
      smoothstep(0.95, 1.0, fract(gridUV.x)),
      smoothstep(0.95, 1.0, fract(gridUV.y))
   )*0.1;
   
   float pulse = sin(time*1.2)*0.5 + 0.5;
   float glow = exp(-length(centeredUV)*3.0)*pulse*0.2;
   vec3 core = glow*vec3(0.3, 0.0, 0.5);
   
   vec3 color = bg + waves + core + vec3(grid);
   color *= 1.0 - length(centeredUV)*0.3;
   
   gl_FragColor = vec4(color, 1.0);
}
