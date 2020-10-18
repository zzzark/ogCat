#pragma once
#include <iostream>
#include "Renderer.h"
#include "shader.h"
#include "vertexBuffer.h"
#include "texture2D.h"
#include "camera.h"
#include "skybox.h"


// ------------------- dbg ------------------- //
#include "dbg.h"
#include "R3DLoader.h"
#include "mesh.h"
#include "frameBuffer.h"
#include "frameEffect.h"
#include "terrain.h"
#include "FXAA.h"
#include "shadowMap2D.h"
#include "TwoPassBlur.h"
#include "gbuffer.h"
#include "shadowBuffer.h"
#include "shadowMapCUBE.h"
#include "feedback.h"
#include "particle.h"

#include "glm/gtc/matrix_transform.hpp"
// ------------------------------------------- //
using namespace cat;
//using cat::shader;
//using cat::renderer;
//using cat::gui;
//using cat::texture2D;
//using cat::camera;
//using cat::skybox;
//using cat::R3DMesh;
//using cat::static_mesh;
//using cat::standardBuffer;
//using cat::frameEffect;
//using cat::terrainBlock;
//using cat::effectFXAA;
//using cat::frameBuffer;
//using cat::shadowMap2D;
//using cat::effectBlur;
//using cat::gbuffer;
//using cat::shadowBuffer;