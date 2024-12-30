#ifndef CYLINDER_H
#define CYLINDER_H

#include "shader.h"
#define M_PI 3.14159265358979323846

class Cylinder {
public:
    GLfloat cylinderVertices[12*48];
    GLfloat cylinderNormals[12*48];
    GLfloat cylinderColors[12*48];
    GLfloat cylinderTexCoords[8*48];
    unsigned int cylinderIndices[6*48];

    unsigned int VAO, VBO, EBO;
    
    unsigned int vSize = sizeof(cylinderVertices);
    unsigned int nSize = sizeof(cylinderNormals);
    unsigned int cSize = sizeof(cylinderColors);
    unsigned int tSize = sizeof(cylinderTexCoords);

    int n;
    bool flat_shading;


    void dynamic_vertice_mapping(int n = 48, bool flat_shading = false) {

        float angle = 2 * M_PI / n;
        for (int i = 0; i < n; i++) {
            cylinderVertices[i * 12] = sin(i * angle);
            cylinderVertices[i * 12 + 1] = -1;
            cylinderVertices[i * 12 + 2] = cos(i * angle);
            cylinderVertices[i * 12 + 3] = sin(i * angle);
            cylinderVertices[i * 12 + 4] = 1;
            cylinderVertices[i * 12 + 5] = cos(i * angle);
            cylinderVertices[i * 12 + 6] = sin((i + 1) * angle);
            cylinderVertices[i * 12 + 7] = 1;
            cylinderVertices[i * 12 + 8] = cos((i + 1) * angle);
            cylinderVertices[i * 12 + 9] = sin((i + 1) * angle);
            cylinderVertices[i * 12 + 10] = -1;
            cylinderVertices[i * 12 + 11] = cos((i + 1) * angle);
        }

        

        for (int i = 0; i < n; i++) {
            cylinderNormals[i * 12] = sin(i * angle);
            cylinderNormals[i * 12 + 1] = 0;
            cylinderNormals[i * 12 + 2] = cos(i * angle);
            cylinderNormals[i * 12 + 3] = sin(i * angle);
            cylinderNormals[i * 12 + 4] = 0;
            cylinderNormals[i * 12 + 5] = cos(i * angle);
            cylinderNormals[i * 12 + 6] = sin((i + 1) * angle);
            cylinderNormals[i * 12 + 7] = 0;
            cylinderNormals[i * 12 + 8] = cos((i + 1) * angle);
            cylinderNormals[i * 12 + 9] = sin((i + 1) * angle);
            cylinderNormals[i * 12 + 10] = 0;
            cylinderNormals[i * 12 + 11] = cos((i + 1) * angle);
        }
        
        

        // smooth shading


        // cube colors are fixed to  (1.0, 0.5, 0.31)
        for (int i = 0; i < n; i++) {

            cylinderColors[i * 12] = 1.0;
            cylinderColors[i * 12 + 1] = 0.5;
            cylinderColors[i * 12 + 2] = 0.31;
            cylinderColors[i * 12 + 3] = 1.0;
            cylinderColors[i * 12 + 4] = 0.5;
            cylinderColors[i * 12 + 5] = 0.31;
            cylinderColors[i * 12 + 6] = 1.0;
            cylinderColors[i * 12 + 7] = 0.5;
            cylinderColors[i * 12 + 8] = 0.31;
            cylinderColors[i * 12 + 9] = 1.0;
            cylinderColors[i * 12 + 10] = 0.5;
            cylinderColors[i * 12 + 11] = 0.31;

        }

        // texture coord array
        for (int i = 0; i < n; i++) {

            cylinderTexCoords[i * 8] = 1;
            cylinderTexCoords[i * 8 + 1] = 1;
            cylinderTexCoords[i * 8 + 2] = 0;
            cylinderTexCoords[i * 8 + 3] = 1;
            cylinderTexCoords[i * 8 + 4] = 0;
            cylinderTexCoords[i * 8 + 5] = 0;
            cylinderTexCoords[i * 8 + 6] = 1;
            cylinderTexCoords[i * 8 + 7] = 0;
        }
    }

    void dynamic_index_mapping(int n) {

        for (int i = 0; i < n; i++) {
            cylinderIndices[i * 6] = i * 4;
            cylinderIndices[i * 6 + 1] = i * 4 + 1;
            cylinderIndices[i * 6 + 2] = i * 4 + 2;
            cylinderIndices[i * 6 + 3] = i * 4 + 2;
            cylinderIndices[i * 6 + 4] = i * 4 + 3;
            cylinderIndices[i * 6 + 5] = i * 4;
        }
    }

    Cylinder(int n = 48) {
        this->n = n;
        dynamic_vertice_mapping(n, false);
        dynamic_index_mapping(n);
        initBuffers();
    }

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize + nSize + cSize + tSize, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, cylinderVertices);
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, cylinderNormals);
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, cylinderColors);
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize + cSize, tSize, cylinderTexCoords);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinderIndices), cylinderIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vSize));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(vSize + nSize));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(vSize + nSize + cSize));
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void draw(Shader *shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, n * 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void render() {
        dynamic_vertice_mapping(n, flat_shading);
        dynamic_index_mapping(n);
        initBuffers();
    }
};

#endif
