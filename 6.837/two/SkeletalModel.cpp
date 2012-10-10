#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
    std::fstream file;
	file.open(filename, std::ios::in);

    float x,y,z;
    int index;
    
    while (!file.eof()){
        file >> x >> y >> z >> index;
        Joint * joint = new Joint;
        joint->transform = Matrix4f::translation(x, y, z);
        m_joints.push_back(joint);
        if (index == -1){
            m_rootJoint = joint;
        } else {
            m_joints[index]->children.push_back(joint);
        }
    }
    file.close();
}

void drawChildrenJoints(Joint * joint, MatrixStack * m_matrixStack){
    /*
     Helper recursive method to draw joints.
     Draw the joint and then the children of the joint.
     */
    m_matrixStack->push(joint->transform);
    glLoadMatrixf(m_matrixStack->top());
    glutSolidSphere(0.025f, 12, 12);
    
    vector<Joint*> children = joint->children;
    for (unsigned i = 0; i < children.size(); i++){
        drawChildrenJoints(children[i], m_matrixStack);
    }
    m_matrixStack->pop();
    glLoadMatrixf(m_matrixStack->top());
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
    drawChildrenJoints(m_rootJoint, &m_matrixStack);
}

void drawChildrenBones(Joint * joint, MatrixStack * m_matrixStack){
    /*
     Helper recursive method to draw bones.
     Draw the bone and then the children of the joint.
     */
    m_matrixStack->push(joint->transform);
    vector<Joint*> children = joint->children;
    for (unsigned i = 0; i < children.size(); i++){
        Joint * child = children[i];
        Vector3f x,y,z;
        z = (child->transform.getCol(3).xyz()).normalized();
        y = (Vector3f::cross(z, Vector3f(0,0,1))).normalized();
        x = (Vector3f::cross(y,z)).normalized();
        m_matrixStack->push(Matrix4f(Vector4f(x,0),
                                     Vector4f(y,0),
                                     Vector4f(z,0),
                                     Vector4f(0,0,0,1)));
        m_matrixStack->push(Matrix4f::scaling(0.05, 0.05,(child->transform.getCol(3).xyz()).abs()));
        m_matrixStack->push(Matrix4f::translation(0, 0, 0.5));
        glLoadMatrixf(m_matrixStack->top());
        glutSolidCube( 1.0f );
        m_matrixStack->pop();
        m_matrixStack->pop();
        m_matrixStack->pop();
        glLoadMatrixf(m_matrixStack->top());
        drawChildrenBones(child, m_matrixStack);
    }
    m_matrixStack->pop();
    glLoadMatrixf(m_matrixStack->top());
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
    drawChildrenBones(m_rootJoint,&m_matrixStack);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
    m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, Matrix3f::rotateX(rX)*Matrix3f::rotateY(rY)*Matrix3f::rotateZ(rZ));
}

void computeChildrenBindWorldToJointTransforms(Joint * joint, MatrixStack * m_matrixStack){
    /*
     Helper recursive method to compute BindWorldToJointTransforms.
     */
    m_matrixStack->push(joint->transform);
    joint->bindWorldToJointTransform = m_matrixStack->top().inverse();
    vector<Joint*> children = joint->children;
    for (unsigned i = 0; i < children.size(); i++){
        computeChildrenBindWorldToJointTransforms(children[i], m_matrixStack);
    }
    m_matrixStack->pop();
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    m_matrixStack.clear();
    computeChildrenBindWorldToJointTransforms(m_rootJoint,&m_matrixStack);
}

void updateChildrenCurrentJointToWorldTransforms(Joint * joint, MatrixStack * m_matrixStack){
    /*
     Helper recursive method to update CurrentJointToWorldTransforms.
     */
    m_matrixStack->push(joint->transform);
    joint->currentJointToWorldTransform = m_matrixStack->top();
    vector<Joint*> children = joint->children;
    for (unsigned i = 0; i < children.size(); i++){
        updateChildrenCurrentJointToWorldTransforms(children[i], m_matrixStack);
    }
    m_matrixStack->pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    m_matrixStack.clear();
    updateChildrenCurrentJointToWorldTransforms(m_rootJoint,&m_matrixStack);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
    
    for (unsigned i = 0; i < m_mesh.bindVertices.size(); i++){
        Vector4f bind_v = Vector4f(m_mesh.bindVertices[i],1);
        float sum_x = 0.;
        float sum_y = 0.;
        float sum_z = 0.;
        float weight_sum = 0.;
        for (unsigned j = 0; j < m_mesh.attachments[i].size(); j++){
            weight_sum += m_mesh.attachments[i][j];
        }
        for (unsigned j = 0; j < m_mesh.attachments[i].size(); j++){
            Matrix4f Tj_Bj = m_joints[j]->currentJointToWorldTransform * m_joints[j]->bindWorldToJointTransform;
            float weight = m_mesh.attachments[i][j];
            sum_x = sum_x + (Tj_Bj * bind_v).x() * weight/weight_sum;
            sum_y = sum_y + (Tj_Bj * bind_v).y() * weight/weight_sum;
            sum_z = sum_z + (Tj_Bj * bind_v).z() * weight/weight_sum;
        }
        m_mesh.currentVertices[i] = Vector3f(sum_x, sum_y, sum_z);
    }
}

