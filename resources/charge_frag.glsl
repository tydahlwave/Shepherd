#version 330 core

in float chargeFrag;

out vec4 Outcolor;

void main() {
    if((gl_FragCoord.x < 100 && gl_FragCoord.x > 10) && (gl_FragCoord.y < 10 * chargeFrag)){
        
        int m = int(gl_FragCoord.y) % 100;
        if(gl_FragCoord.y > (10 * chargeFrag) - 10)
        {
            Outcolor = vec4(0, 0, 0, 1);
        }
        else{
            Outcolor = mix(vec4(1, 0, 0, 1), vec4(1, 1, 0, 1), gl_FragCoord.y/ 2000);
        }
    }
    else if((gl_FragCoord.x < 10 || (gl_FragCoord.x > 100) && gl_FragCoord.x < 110) && (gl_FragCoord.y < 10 * chargeFrag))
    {
        Outcolor = vec4(0, 0, 0, 1);
    }else {
        discard;
    }
}
