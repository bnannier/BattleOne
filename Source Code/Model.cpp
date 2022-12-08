///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Model.cpp
//	Start Date: 07/02/2006
//	End Date:   12/03/2006
//
//	Assignment:	BattleOne
//	Subject:    GAM666, GAM670, GAM671
//	Programmer: Bobby Nannier
//	Student #:  020 208 013
//	Instructor: Evan Weaver			- GAM666 (Introduction To Game Programming)
//									- GAM670 (Game Programming Techniques)
//				Chris Szalwinski	- GAM671 (Physics)
//				Catherine Leung		- GAM671 (Artificial Intelligence)
//
//	Purpose:	
//
//	Notes:		
///////////////////////////////////////////////////////////////////////////////////////

#include "Model.h"
#include "Thing.h"
#include "Utility.h"  // for logMsg()
#include <stdio.h>      // for sprintf

// ----------------------------------------------------------------------------------------------------
// Model reads the vertex and triangle data from dataFile and if successful
// stores the data for constructing a Thing, if unsuccessful does not store any data.
// dump == true turns on dumps to dump.log of model data for debugging purposes
// ----------------------------------------------------------------------------------------------------
Model::Model(const char* dataFile, bool dump) {

	FILE* fp = NULL;
	char msg[121], line[121], label[121], value[121];
	int rc;

	noFaces = 0;
	noVertices = 0;
	normalData = false;
	dumpBuild = dump;
	vertex  = NULL;
	fNormal = NULL;
	vNormal = NULL;
	texture = NULL;
	texFile = NULL;
	faceVertex1 = NULL;
	faceVertex2 = NULL;
	faceVertex3 = NULL;

	fp = fopen(dataFile, "r");
	if (!fp) {
		sprintf(msg, "Failed to open %s", dataFile);
		logMsg(msg);
		success = false;
	} else {
		success = true;
		// skip the first line of the file
		fscanf(fp, "%*[^\n]%*c");
		do {
			// read the subsection identifier
			value[0] = '\0';
			label[0] = '\0';
			rc = fscanf(fp, " %120[^\n]%*c", line);
			if (rc != EOF && line[0] != ';' && line[0] != '#') {
				// extract the label and the data value
			    sscanf(line, "%120s%*[^=:]%*c%120s", &label, &value);
			    // extract the data values for the subsection
			    success = success && extract(fp, dataFile, label, value);
			}
		} while (success && rc != EOF);
		fclose(fp);
		// need at least the vertex and the face data
		if (!vertex || !faceVertex1)
		    success = false;
		if (!success) release();
	}
}

// ----------------------------------------------------------------------------------------------------
// Extract reads data values for the subsection named label
// ----------------------------------------------------------------------------------------------------
bool Model::extract(FILE* fp, const char* dataFile, const char* label,
 const char* value) {

	bool success = true;
	char msg[81];

	if (!strcmp(label, "Vertices")) {
		noVertices = 0;
		if (sscanf(value, "%d", &noVertices)) {
		    vertex  = new D3DXVECTOR3[noVertices];
		    if (!vNormal) vNormal = new D3DXVECTOR3[noVertices];
			// read noVertices set of triples in the form [ , , ],
			for (int i = 0; i < noVertices; i++) {
				if (fscanf(fp," [%f,%f,%f],", &vertex[i].x, &vertex[i].y,
				 &vertex[i].z) != 3) {
	    	        sprintf(msg, "Failed to read a vertex from %s", dataFile);
			        logMsg(msg);
					i = noVertices;
					success = false;
				}
			}
		} else {
		    success = false;
			logMsg("Failed to read number of Vertices");
		}
	} else if (!strcmp(label, "Faces")) {
		noFaces = 0;
		if (sscanf(value, "%d", &noFaces)) {
            faceVertex1 = new int[noFaces];
            faceVertex2 = new int[noFaces];
            faceVertex3 = new int[noFaces];
            fNormal = new D3DXVECTOR3[noFaces];
		    if (!texture) texture = new D3DXVECTOR3[noFaces];
			// read noFaces set of int triples in the form [ , , ],
			for (int i = 0; i < noFaces; i++) {
				if (fscanf(fp," [%d,%d,%d],", &faceVertex1[i], &faceVertex2[i],
				 &faceVertex3[i]) != 3) {
					sprintf(msg, "Failed to read a face from %s", dataFile);
					logMsg(msg);
					i = noFaces;
					success = false;
				}
			}
		} else {
			success = false;
			logMsg("Failed to read number of Faces");
		}
	} else if (!strcmp(label, "Normals")) {
		noVertices = 0;
		if (sscanf(value, "%d", &noVertices)) {
		    if (!vNormal) vNormal = new D3DXVECTOR3[noVertices];
			// read noVertices set of triples in the form [ , , ],
			for (int i = 0; i < noVertices; i++) {
				if (fscanf(fp," [%f,%f,%f],", &vNormal[i].x, &vNormal[i].y,
				 &vNormal[i].z) != 3) {
	    	        sprintf(msg, "Failed to read a normal from %s", dataFile);
			        logMsg(msg);
					i = noVertices;
					success = false;
				}
			}
		} else {
			success = false;
			logMsg("Failed to read number of Vertices");
		}
	} else if (!strcmp(label, "Colours")) {
        ;
	} else if (!strcmp(label, "Textures")) {
		if (value[0] != '\0') {
		    texFile = new char[strlen(value) + 1];
		    strcpy(texFile, value);
		    if (!texture) texture = new D3DXVECTOR3[noFaces];
		    // read noFaces set of triples in the form [ , , ],
		    for (int i = 0; i < noFaces; i++) {
				int rc;
			    if ((rc = fscanf(fp," [%f,%f,%f],", &texture[i].x, &texture[i].y,
			     &texture[i].z)) != 3 && rc != EOF) {
				    sprintf(msg, "Failed to read a texture from %s %d %d ", dataFile, i, rc);
				    logMsg(msg);
				    i = noFaces;
				    success = false;
			    }
		    }
		}
    } else {
		logMsg(label);
		logMsg(value);
        logMsg("Unrecognizable data <- ignored");
	}

    return success;
}

// ----------------------------------------------------------------------------------------------------
// Release deallocates all dynamic memory for the model
// ----------------------------------------------------------------------------------------------------
void Model::release() {

	if (vertex)  delete [] vertex;
	if (fNormal) delete [] fNormal;
	if (vNormal) delete [] vNormal;
	if (texture) delete [] texture;
	if (texFile) delete [] texFile;
	if (faceVertex1) delete [] faceVertex1;
	if (faceVertex2) delete [] faceVertex2;
	if (faceVertex3) delete [] faceVertex3;
	vertex = vNormal = fNormal = texture = NULL;
	faceVertex1 = faceVertex2 = faceVertex3 = NULL;
}

// ----------------------------------------------------------------------------------------------------
// Build constructs a Thing from the data held within the Model
// ----------------------------------------------------------------------------------------------------
void Model::build(Thing* thing, FLOAT scale, bool smooth) {

    int a, b, c, count;
    char msg[21];
    D3DXVECTOR3 x, y, z, average;

    if (!normalData) {
        // assume that normals are normal to each triangle face
        for (int j = 0; j < noFaces; j++) {
            // Triangle List
            // face data starts from vertex #1
            a = faceVertex1[j] - 1;
            b = faceVertex2[j] - 1;
            c = faceVertex3[j] - 1;
            x = vertex[b] - vertex[a];
            y = vertex[c] - vertex[b];
            D3DXVec3Cross(&z, &x, &y);
            D3DXVec3Normalize(&fNormal[j], &z);
		}
        if (smooth) {
			// average normals on faces that meet at a vertex
            for (int i = 0; i < noVertices; i++) {
                count = 0;
                for (int j = 0; j < noFaces; j++) {
                    if (faceVertex1[j] - 1 == i) {
                        average += fNormal[j];
                        count++;
                    } else if (faceVertex2[j] - 1 == i) {
                        average += fNormal[j];
                        count++;
                    } else if (faceVertex3[j] - 1 == i) {
                        average += fNormal[j];
                        count++;
                    }
                }
                if (count)
                    vNormal[i] = average / (float)count;
                else
                    vNormal[i] = D3DXVECTOR3(0, 0, 0);
    		}
    		// add vertices to Vertex Buffer with averaged normals
            for (int j = 0; j < noFaces; j++) {
                // Triangle List
                // face data starts from vertex #1
                a = faceVertex1[j] - 1;
                b = faceVertex2[j] - 1;
                c = faceVertex3[j] - 1;
                thing->add(scale * vertex[a].x, scale * vertex[a].y,
                 scale * vertex[a].z, vNormal[a].x, vNormal[a].y, vNormal[a].z,
                 0, 0, 0);
                thing->add(scale * vertex[b].x, scale * vertex[b].y,
                 scale * vertex[b].z, vNormal[b].x, vNormal[b].y, vNormal[b].z,
                 0, texture[j].y, 0);
                thing->add(scale * vertex[c].x, scale * vertex[c].y,
                 scale * vertex[c].z, vNormal[c].x, vNormal[c].y, vNormal[c].z,
                 texture[j].x, texture[j].y, 1);
                if (dumpBuild) {
                    sprintf(msg, ": vertex %d", a);
                    logVec(msg, &vertex[a]);
                    sprintf(msg, ": vertex %d", b);
                    logVec(msg, &vertex[b]);
                    sprintf(msg, ": vertex %d", c);
                    logVec(msg, &vertex[c]);
                }
            }
        } else {
			// use generated normals without smoothing
            for (int j = 0; j < noFaces; j++) {
                // Triangle List
                // face data starts from vertex #1
                a = faceVertex1[j] - 1;
                b = faceVertex2[j] - 1;
                c = faceVertex3[j] - 1;
                thing->add(scale * vertex[a].x, scale * vertex[a].y,
                 scale * vertex[a].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
                 0, 0, 0);
                thing->add(scale * vertex[b].x, scale * vertex[b].y,
                 scale * vertex[b].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
                 0, texture[j].y, 0);
                thing->add(scale * vertex[c].x, scale * vertex[c].y,
                 scale * vertex[c].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
                 texture[j].x, texture[j].y, 1);
                if (dumpBuild) {
                    sprintf(msg, ": vertex %d", a);
                    logVec(msg, &vertex[a]);
                    sprintf(msg, ": vertex %d", b);
                    logVec(msg, &vertex[b]);
                    sprintf(msg, ": vertex %d", c);
                    logVec(msg, &vertex[c]);
                }
            }
    	}
	} else {
		// use normals as supplied
		for (int j = 0; j < noFaces; j++) {
			// Triangle List
			// face data starts from vertex #1
			a = faceVertex1[j] - 1;
			b = faceVertex2[j] - 1;
			c = faceVertex3[j] - 1;
			thing->add(scale * vertex[a].x, scale * vertex[a].y,
			 scale * vertex[a].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
			 0, 0, 0);
			thing->add(scale * vertex[b].x, scale * vertex[b].y,
			 scale * vertex[b].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
			 0, texture[j].y, 0);
			thing->add(scale * vertex[c].x, scale * vertex[c].y,
			 scale * vertex[c].z, fNormal[j].x, fNormal[j].y, fNormal[j].z,
			 texture[j].x, texture[j].y, 1);
			if (dumpBuild) {
				sprintf(msg, ": vertex %d", a);
				logVec(msg, &vertex[a]);
				sprintf(msg, ": vertex %d", b);
				logVec(msg, &vertex[b]);
				sprintf(msg, ": vertex %d", c);
				logVec(msg, &vertex[c]);
			}
		}
	}
}
