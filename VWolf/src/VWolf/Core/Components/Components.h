//
//  Component.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/11/23.
//

#pragma once

#include "BaseComponent.h"
#include "TransformComponent.h"
#include "RendererComponent.h"
#include "MeshFilterComponent.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "RigidBodyComponent.h"
#include "MeshColliderComponent.h"
#include "SphereColliderComponent.h"
#include "BoxColliderComponent.h"
#include "AudioListenerComponent.h"
#include "AudioSourceComponent.h"

#include <boost/mpl/list.hpp>

using AllComponents = boost::mpl::list<
                        VWolf::TransformComponent,
                        VWolf::ShapeRendererComponent,
                        VWolf::MeshFilterComponent,
                        VWolf::MeshRendererComponent,
                        VWolf::LightComponent,
                        VWolf::CameraComponent,
                        VWolf::RigidBodyComponent,
                        VWolf::MeshColliderComponent,
                        VWolf::SphereColliderComponent,
                        VWolf::BoxColliderComponent,
                        VWolf::AudioListenerComponent,
                        VWolf::AudioSourceComponent
                    >;
