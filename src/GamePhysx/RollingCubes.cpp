#include "ODEWorld.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

using namespace std;
using namespace chrono;

#define DENSITY (.5f)

#define MAX_ROBOTS_NUM 16*16

enum COLOR_RGB {
    R = 0,
    G = 1,
    B = 2
};

typedef struct {
    dBodyID body;
    dGeomID geom;
} GameObject;

typedef struct {
    dBodyID body;
    high_resolution_clock::time_point event_time;
} ColorItem;

static dWorldID world;  // hold the rigidbodies for display
static dSpaceID space;  // hold the geometries for collision detection
static dGeomID ground;
static dJointGroupID contactgroup;
static int flag = 0;
static dsFunctions fn;
static const dReal radius = 0.2;
static const dReal min_dist = 5.0f;
static const dReal sides[3] = {2.0f, 2.0f, 2.0f};
static const dReal sides_robot[3] = {1.0f, 1.0f, 1.0f};
static const dReal roll_power = 250.0f;
static const dReal push_power = 50.0f;
static const dReal jump_power = 10;
static const dReal main_color[3] = {1.0 - 245.0f/255.0f, 1.0 - 140.0f/255.0f, 1.0 - 34.0f/255.0f};
static std::unordered_map<dBodyID, high_resolution_clock::time_point> color_list;
static GameObject box;
static GameObject robots[MAX_ROBOTS_NUM];

static const int WIDTH = 900;
static const int HEIGHT = 500;


static void nearCallback(void *data, dGeomID o1, dGeomID o2) {
    const int N = 10;
    dContact contact[N];

    int isGround = ((o1 == ground) || (o2 == ground));
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    if (isGround){
        if (n >= 1) flag = 1; else flag = 0;
        for (int i = 0; i < n; i++) {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.bounce = 0.2f;
            contact[i].surface.bounce_vel = 0.001f;
            dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
            dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
        }
    } else {
        
        for (int i = 0; i < n; i++) {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.bounce = 0.2f;
            contact[i].surface.bounce_vel = 0.001f;
            dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
            dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));

            color_list[dGeomGetBody(contact[i].geom.g1)] = high_resolution_clock::now();
            color_list[dGeomGetBody(contact[i].geom.g2)] = high_resolution_clock::now();
        }
    }
}

static void findNearestBoxes() {
    for (int i = 0; i < 256; i++) {
        const dReal* robot_pos = dBodyGetPosition(robots[i].body);
        const dReal* box_pos = dBodyGetPosition(box.body);
        dReal dist = dDISTANCE(robot_pos, box_pos);
        
        if (dist < min_dist) {            
            double force_dir[3];
            dSubtractVectors3(force_dir, robot_pos, box_pos);
            dNormalize3(force_dir);
            dBodySetForce(robots[i].body, push_power * force_dir[0], push_power * force_dir[1], push_power * force_dir[2]);
        }
    }
}


static void step(int pause) {
    const dReal *pos, *R;
    flag = 0; 
    dSpaceCollide(space, 0, &nearCallback);   

    dWorldStep(world, 0.01);
    dJointGroupEmpty(contactgroup);
    
    dsSetColor(1.0 - 245.0f/255.0f, 1.0 - 140.0f/255.0f, 1.0 - 34.0f/255.0f);

    pos = dBodyGetPosition(box.body);
    R = dBodyGetRotation(box.body);
    dsDrawBoxD(pos, R, sides);

    float new_xyz[3] = {pos[dVec3Element::dV3E_X] + 15.0f, pos[dVec3Element::dV3E_Y], 
        pos[dVec3Element::dV3E_Z] + 4.0f};

    float hpr[3] = {180.0f, -11.0f, 0.0f};
    dsSetViewpoint(new_xyz, hpr);

    findNearestBoxes();

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            dBodyID curr_bodyId = robots[(i * 16) + j].body;
            if (color_list.find(curr_bodyId) != color_list.end()) {
                high_resolution_clock::time_point event_time = color_list[curr_bodyId];

                int64_t elapsed_ms = duration_cast<milliseconds>(
                    high_resolution_clock::now() - event_time).count();
                
                float percent = std::min(1.0f, elapsed_ms * 1.0f / 3000.0f);
                float rest_percent = 1.0f - percent;

                dsSetColor(
                    ((1.0 - 245.0f/255.0f)) * rest_percent, 
                    ((1.0 - 140.0f/255.0f)) * rest_percent, 
                    ((1.0 - 34.0f/255.0f)) * rest_percent
                );
                if (percent >= 0.8f) {
                    color_list.erase(curr_bodyId);
                }
            } else {
                dsSetColor(0.9, 0.9, 0.9);
            }            

            const dReal* tmp_p = dBodyGetPosition(curr_bodyId);
            double new_p[3] = { tmp_p[dVec3Element::dV3E_X], tmp_p[dVec3Element::dV3E_Y], 0.5f};
            dsDrawBoxD(new_p, dBodyGetRotation(curr_bodyId), sides_robot);
            // dsDrawBox(
            //     new_p, 
            //     dBodyGetRotation(curr_bodyId), 
            //     sides_robot);
        }
    }
}

static void start() {
    static float xyz[3] = {15.0f, 0.0f, 4.0f};
    static float hpr[3] = {180.0f, -11.0f, 0.0f};
    dsSetViewpoint(xyz, hpr);

    printf("-------------------------------------\n");
    printf("press 'a' for going left\n");
    printf("press 'd' for going right\n");
    printf("press 'w' for going forward\n");
    printf("press 's' for going backward\n");
    printf("-------------------------------------\n");
}

static void command(int cmd) {
    // dBodySetForce(box.body, 0.0f, 0.0f, 0.0f);
    // dBodySetTorque(box.body, 0.0f, 0.0f, 0.0f);
    const dReal* relpos = dBodyGetPosition(box.body);
    switch (cmd)
    {
    case 'w':
        // dBodySetTorque(box.body, 0.0f, -roll_power, 0.0f);
        // dBodySetForce(box.body, -roll_power, 0.0f, 0.0f);
        dBodyAddForceAtPos(box.body, -roll_power, 0.0f, 0.0f, relpos[0], relpos[1], relpos[2] + 2.0f);
        break;
    case 's':

        // dBodySetTorque(box.body, 0.0f, roll_power, 0.0f);
        // dBodySetForce(box.body, roll_power, 0.0f, 0.0f);
        dBodyAddForceAtPos(box.body, roll_power, 0.0f, 0.0f, relpos[0], relpos[1], relpos[2] + 2.0f);
        break;
    
    case 'a':
        // dBodySetTorque(box.body, roll_power, 0.0f, 0.0f);
        // dBodySetForce(box.body, 0.0f, -roll_power, 0.0f);
        dBodyAddForceAtPos(box.body, 0.0f, -roll_power, 0.0f, relpos[0], relpos[1], relpos[2] + 2.0f);
        break;

    case 'd':
        //dBodySetTorque(box.body, -roll_power, 0.0f, 0.0f);
        // dBodySetForce(box.body, 0.0f, roll_power, 0.0f);        
        dBodyAddForceAtPos(box.body, 0.0f, roll_power, 0.0f, relpos[0], relpos[1], relpos[2] + 2.0f);
        break;

    case 'q':
        dBodyAddForce(box.body, 0, 0, jump_power*10);
        break;

    case 'e':
        dBodyAddForce(box.body, 0, 0, -jump_power*10);
        break;

    default:
        break;
    }
}

static void InitDrawstuff() {
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &step;
    fn.command = &command;
    fn.stop = 0;
    fn.path_to_textures = "../resources/ode/textures";
}

void InitRollingCubesWorld(int argc, char** argv) {
    dReal x0 = 0.0f, y0 = 0.0f, z0 = 10.0f;
    dMass m1;

    InitDrawstuff();

    dInitODE();

    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    
    dWorldSetERP(world, 0.2f);
    dWorldSetCFM(world, 0.0005f);

    dWorldSetGravity(world, 0, 0, -9.8f);
    ground = dCreatePlane(space, 0, 0, 1, 0);
    
    box.body = dBodyCreate(world);
    dMassSetZero(&m1);
    dMassSetBox(&m1, DENSITY, sides[0], sides[1], sides[2]);
    dBodySetMass(box.body, &m1);
    dBodySetPosition(box.body, x0, y0, z0);
    box.geom = dCreateBox(space, sides[0], sides[1], sides[2]);
    dGeomSetBody(box.geom, box.body); // bind the body with geom
        
    // create robots
    int count = 0;
    memset(robots, 0, sizeof(GameObject) * 256);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            dMass robot_mass;
            int idx = (i * 16) + j;
            robots[idx].body = dBodyCreate(world);
            dMassSetZero(&robot_mass);
            dMassSetBox(&robot_mass, DENSITY, sides_robot[0], sides_robot[1], sides_robot[2]);
            dBodySetMass(robots[idx].body, &robot_mass);

            dReal x = (i * 1.0f * 2.0f) - 15.0f;
            dReal y = (1.0f * 2.0f * j) - 15.0f;
            dBodySetPosition(robots[idx].body, x, y, 0.0f);

            robots[idx].geom = dCreateBox(space, sides_robot[0], sides_robot[1], sides_robot[2]);
            dGeomSetBody(robots[idx].geom, robots[idx].body);
            count++;
            printf("robot box[%d,%d:%d]: x: %f, y: %f\n", i, j, idx, x, y);
        }
    }

    printf("%d robot box created\n", count);

    dsSimulationLoop(argc, argv, WIDTH, HEIGHT, &fn);
    dWorldDestroy(world);

    dCloseODE();
}