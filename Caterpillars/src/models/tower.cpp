#include "tower.hpp"

void TowerLight::moveLight(){
   this->direction == 0 ? this->rot.y+=0.05 : this->rot.y -=0.05;
   if (this->rot.y > this->endA) this->direction=1;
   if (this->rot.y < this->startA) this->direction=0;

   this->recalculateMatrix();
}

TowerLight::TowerLight(char*filename,Object *o, float startA, float endA, int dir){
   this->shader = new Shader("../src/shaders/catterShader.vs","../src/shaders/catterShader.frag");

   this->scl.x=8;
   this->scl.y=8;
   this->scl.z=8;

   this->pos.x=o->pos.x;
   this->pos.y=o->pos.y;
   this->pos.z=o->pos.z;
   this->pos.y+=(2.8*o->scl.y);

   this->rot.x=0;
   this->rot.z=0;
   this->rot.y=startA*2*3.14/360;

   this->startA=startA*2*3.14/360;
   this->endA=endA*2*3.14/360;

   this->direction=dir;

   this->recalculateMatrix();

   loadObj::load(filename,&this->vertices, &this->indices);
   this->bindBuffers(5,GL_STATIC_DRAW);
   this->bindTexture2D("../src/img/towerL.png");
   puts("created tower light");

}
TowerLight::~TowerLight(){};
void TowerLight::draw(glm::mat4 projection, glm::mat4 modelView){
   this->shader->useShaderProgram(0);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, this->texture2D);
   glUniform1i(glGetUniformLocation(this->shader->shaderProgram[0], "ourTexture1"), 0);

   GLint P = glGetUniformLocation(this->shader->shaderProgram[0], "P");
   GLint V = glGetUniformLocation(this->shader->shaderProgram[0], "V");
   GLint M = glGetUniformLocation(this->shader->shaderProgram[0], "M");

   glUniformMatrix4fv(P, 1, GL_FALSE, glm::value_ptr(projection));
   glUniformMatrix4fv(V, 1, GL_FALSE, glm::value_ptr(modelView));
   glUniformMatrix4fv(M, 1, GL_FALSE, glm::value_ptr(this->posM * this->sclM * this->rotM));
   //
   glBindVertexArray(this->currentVAO());
 	// //glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
   glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
   glBindVertexArray(0);
}

Tower::Tower(char *filename, int posX, int posY, int posZ, int startAngle, int endAngle){

   this->shader = new Shader("../src/shaders/catterShader.vs","../src/shaders/catterShader.frag");

   this->scl.x=10;
   this->scl.y=10;
   this->scl.z=10;

   this->pos.x=posX;
   this->pos.y=posY;
   this->pos.z=posZ;

   this->recalculateMatrix();

   loadObj::load(filename,&this->vertices, &this->indices);
   this->bindBuffers(5,GL_STATIC_DRAW);
   this->bindTexture2D("../src/img/tower.png");
   puts("created tower");

   this->light = new TowerLight((char*)"../src/obj/towerL.obj",this,(float)startAngle,(float)endAngle,0);
}
Tower::~Tower(){

}

void Tower::draw(glm::mat4 projection, glm::mat4 modelView){
  this->shader->useShaderProgram(0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture2D);
  glUniform1i(glGetUniformLocation(this->shader->shaderProgram[0], "ourTexture1"), 0);

  GLint P = glGetUniformLocation(this->shader->shaderProgram[0], "P");
  GLint V = glGetUniformLocation(this->shader->shaderProgram[0], "V");
  GLint M = glGetUniformLocation(this->shader->shaderProgram[0], "M");

  glUniformMatrix4fv(P, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(V, 1, GL_FALSE, glm::value_ptr(modelView));
  glUniformMatrix4fv(M, 1, GL_FALSE, glm::value_ptr(this->posM * this->sclM));
  //
  glBindVertexArray(this->currentVAO());
	// //glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
  glBindVertexArray(0);

  this->light->draw(projection,modelView);
}