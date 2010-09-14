#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform vec3 planeNormal;

uniform float delta;
uniform int sequence[64];
uniform vec3 vertices[8];
uniform vec4 brickMin;
uniform vec4 brickDimInv;
uniform vec3 texMin;
uniform vec3 texRange;
uniform int v1[24];
uniform int v2[24];

void main()
{
  float planeDist = brickMin.w + gl_PositionIn[0].y * delta;
  vec3 pos;

  for (int v=0; v<6; ++v)
  {
  for (int i=0; i<4; ++i)
  {
    // int(brickDimInv[3] == frontIndex.
    int vIdx1 = sequence[int(brickDimInv[3] * 8 + v1[v*4+i])];
    int vIdx2 = sequence[int(brickDimInv[3] * 8 + v2[v*4+i])];

    vec3 vecV1 = vertices[vIdx1];
    vec3 vecV2 = vertices[vIdx2];

    vec3 vecStart = vecV1;
    vec3 vecDir = vecV2-vecV1;

    float denominator = dot(vecDir, planeNormal);
    float lambda = (denominator != 0.0) ?
                      (planeDist-dot(vecStart, planeNormal)) / denominator
                   :
                     -1.0;

    if ((lambda >= 0.0) && (lambda <= 1.0))
    {
      pos = vecStart + lambda * vecDir;
      break;
    }
  }
  
  gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
  gl_TexCoord[0].xyz = (pos - brickMin.xyz) * brickDimInv.xyz;
  gl_TexCoord[0].xyz = gl_TexCoord[0].xyz * texRange + texMin;
  EmitVertex();
  }
  EndPrimitive();
}
