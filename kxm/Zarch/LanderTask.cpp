//
//  File.cpp
//  kxm
//
//  Created by Kai Hergenroether on 4/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include <kxm/Zarch/LanderTask.h>

#include <kxm/Core/NumberTools.h>
#include <kxm/Vectoid/Transform.h>
#include <kxm/Vectoid/CoordSysInterface.h>
#include <kxm/Zarch/MapParameters.h>

using boost::shared_ptr;
using namespace kxm::Core;
using namespace kxm::Vectoid;
using namespace kxm::Game;
using namespace kxm::Zarch;


namespace kxm {
namespace Zarch {

LanderTask::LanderTask(shared_ptr<CoordSysInterface> landerCoordSys,
                       shared_ptr<const FrameTimeTask::FrameTimeInfo> timeInfo,
                       shared_ptr<const Vector> accelerometerGravity,
                       shared_ptr<MapParameters> mapParameters)
        : landerCoordSys_(landerCoordSys),
          timeInfo_(timeInfo),
          accelerometerGravity_(accelerometerGravity),
          mapParameters_(mapParameters),
          landerState_(new LanderStateInfo()),
          heading_(0.0f, 0.0f, -1.0f) {
}

void LanderTask::FireThruster(bool thrusterEnabled) {
    landerState_->thrusterEnabled = thrusterEnabled;
}

shared_ptr<const LanderTask::LanderStateInfo> LanderTask::LanderState() {
    return landerState_;
}

void LanderTask::Execute() {
    landerState_->lastFramePosition = landerState_->transform.TranslationPart();
    landerState_->lastFrameVelocity = landerState_->velocity;
    
    float projection = accelerometerGravity_->x;
    NumberTools::Clamp(&projection, -1.0f, 1.0f);
    float xAngle = (float)asin(projection) * 180.0f / 3.141592654f;
    projection = accelerometerGravity_->y;
    NumberTools::Clamp(&projection, -1.0f, 1.0f);
    float yAngle = -(float)asin(projection) * 180.0f / 3.141592654f;
    float maxAngle = 30.0f;
    NumberTools::Clamp(&xAngle, -maxAngle, maxAngle);
    NumberTools::Clamp(&yAngle, -maxAngle, maxAngle);
    Vector speed(xAngle / maxAngle, 0.0f, yAngle / maxAngle);
    float  speedLength = speed.Length();
    if (speedLength > 0.0f)
        heading_ = (1.0f/speedLength) * speed;
    NumberTools::Clamp(&speedLength, 0.0f, 1.0f);
    
    Vector up(0.0f, 1.0f, 0.0f);
    Transform newLanderTransform(CrossProduct(up, -heading_), up, -heading_);
    newLanderTransform.Prepend(Transform(XAxis, -speedLength * 120.0f));
    newLanderTransform.Prepend(Transform(YAxis, 180.0));
    
    // Apply gravity...
    landerState_->velocity.y += timeInfo_->timeSinceLastFrame * -mapParameters_->gravity;
    // Apply thrust...?
    if (landerState_->thrusterEnabled) {
        Vector thrustDirection(0.0f, 1.0f, 0.0f);
        newLanderTransform.ApplyTo(&thrustDirection);
        landerState_->velocity += (  timeInfo_->timeSinceLastFrame
                                   * mapParameters_->landerThrust ) * thrustDirection;
    }
    Vector position = landerState_->transform.TranslationPart();
    position += timeInfo_->timeSinceLastFrame * landerState_->velocity;
    newLanderTransform.SetTranslationPart(position);
    
    landerCoordSys_->SetTransform(newLanderTransform);
    landerState_->transform = newLanderTransform;
}

}    // Namespace Zarch.
}    // Namespace kxm.

