// You can use the entire C/C++ standard library, just add the relevant
// #includes. We recommend math.h ;-)

#include <math.h>
#include <bits/stdc++.h> 

#include "usercode.h"

ipc_real_t eatenFood;
int ticksWithoutFood = 0;
int ticksWithBoost = 0;

#define TICKS_BEFORE_FOOD_BOOST = 100
#define BOOST_TICKS = 5

/*
 * This is your bot's startup function. Here you can set your snake's colors,
 * set up persistent variables, etc.
 */
bool init(Api *api)
{
    // remove the default color
    api->clearColors();

    api->addColor(221, 110, 52);
    api->addColor(41, 41, 41);

    eatenFood = api->getSelfInfo()->consumed_natural_food;

    // indicate successful startup. If anything goes wrong,
    // return false and we'll clean you up.
    return true;
}

IpcFoodInfo getClosestFood(const IpcFoodInfo *foods, size_t foodCount)
{
    ipc_real_t minDistance = foods[0].dist;
    IpcFoodInfo closestFood = foods[0];

    for(size_t i = 0; i < foodCount; i++) {
        const IpcFoodInfo food = foods[i];

        if(food.dist < minDistance) {
            closestFood = food;
            minDistance = food.dist;
        }
    }

    return closestFood;
}

ipc_real_t getClosestFoodDirection(const IpcFoodInfo *foods, size_t foodCount)
{
    IpcFoodInfo closestFood = getClosestFood(foods, foodCount);

    return closestFood.dir;
}

ipc_real_t getFoodDirection(Api *api)
{
    if(api->getFoodCount() == 0) {
        return (ipc_real_t)0;
    }

    const IpcFoodInfo *foods = api->getFood();

    ipc_real_t newDirection = getClosestFoodDirection(foods, api->getFoodCount());

    return newDirection;
}

ipc_real_t getCurrentDirection(Api *api)
{
    const IpcSegmentInfo *segments = api->getSegments();
    size_t segmentCount = api->getSegmentCount();

    IpcSegmentInfo head = segments[0];
    ipc_real_t minIdx = segments[0].idx;

    for(size_t i = 0; i < segmentCount; i++) {
        IpcSegmentInfo segment = segments[i];

        if(!segment.is_self) {
            continue;
        }

        if(segment.idx < minIdx) {
            head = segment;
        }
    }

    return head.dir;
}

/*
 * This function will be called by the framework on every step. Here you decide
 * where to move next!
 *
 * Use the provided Api object to interact with the world and make sure you set
 * the following outputs:
 *
 * - api->angle: Set your relative movement angle
 * - api->boost: Set this to true to move faster, but you will loose mass.
 *
 * The Api object also provides information about the world around you. See the
 * documentation for more details.
 */
bool step(Api *api)
{
    // let's start by moving in a large circle. Please note that all angles are
    // represented in radians, where -π to +π is a full circle.
    //api->angle = 0.001;

    // check for other snakes
    //for(size_t i = 0; i < api->getSegmentCount(); i++) {
    //    const IpcSegmentInfo &seg = api->getSegments()[i];

    //    if(!seg.is_self && seg.dist < 20) {
    //        // you can send log messages to your browser or any other viewer with the
    //        // appropriate Viewer Key.
    //        api->log("Oh no, I'm going to die!");
    //        break;
    //    }
    //}

    ipc_real_t currentDirection = getCurrentDirection(api);
    ipc_real_t newDirection = getFoodDirection(api);

    //api->log(std::to_string(newDirection).c_str());

    if(newDirection > 0) {
        api->angle = std::min(newDirection, api->getSelfInfo()->max_step_angle);
    } else {
        api->angle = std::max(newDirection, -api->getSelfInfo()->max_step_angle);
    }

    // Signal that everything is ok. Return false here if anything goes wrong but
    // you want to shut down cleanly.
    return true;
}