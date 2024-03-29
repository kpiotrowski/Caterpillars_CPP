#include "particle.hpp"
#include "../../inputActions.hpp"

//GLfloat ParticleEffect::particleEffectVertex[12] = {-0.5f,-0.5f,-0.3f,0.5f,-0.5f,0.0f,-0.5f,0.5f,0.0f,0.5f,0.5f,-0.3f};
GLfloat ParticleEffect::particleEffectVertex[18] = {  -0.5f,-0.5f,0.0f,
                                                      0.5f,-0.5f,0.0f,
                                                      0.0f,0.35f,0.0f,
                                                      0.0f,-0.08f,0.85f,
                                                      -0.5f,-0.5f,0.0f,
                                                      0.5f,-0.5f,0.0f};


ParticleEffect::ParticleEffect(glm::vec3 pos, float maxTime, float minSize, float maxSize, int maxParticles, float life, float coef){

   this->effectPos = pos;
   this->effectMaxTime=maxTime;
   this->effectTimeLeft=maxTime;
   this->effectMinSize=minSize;
   this->effectMaxSize=maxSize;
   this->effectMaxParticles=maxParticles;
   this->effectParticlesLeft=maxParticles;
   this->particlesCount=0;
   this->singleParticleLife = life;

   this->effectParticlePosAndSize = new GLfloat[4*this->effectMaxParticles];
   this->effectColorData = new GLfloat[2*this->effectMaxParticles];
   for(int i=0; i< effectMaxParticles*4;i++) effectParticlePosAndSize[i]=0;
   for(int i=0; i< effectMaxParticles*2;i++) effectColorData[i]=0;

   puts("created particle effect");

   glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   std::vector<unsigned char> image;
	unsigned width,height;
	unsigned error = lodepng::decode(image,width,height,"../src/img/fire.png");
	if(error != 0) std::cout << "ERROR:: " << error << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);
   errorCheck("Po loadTexture");

   srand( time( NULL ) );
   this->createFirstParticles(coef);

   glGenVertexArrays(1, &this->VAO);
   glBindVertexArray(this->VAO);

   this->shader = new Shader("../src/shaders/particle.vs","../src/shaders/particle.frag");

   this->bindBuffers();
}

void ParticleEffect::bindBuffers(){
   this->VerBuffer=0;
   this->ParPosBuffer=0;
   this->ParColBuffer=0;

   glGenBuffers(1, &this->VerBuffer);
   glGenBuffers(1, &this->ParPosBuffer);
   glGenBuffers(1, &this->ParColBuffer);


   glBindBuffer(GL_ARRAY_BUFFER, this->VerBuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(particleEffectVertex), particleEffectVertex, GL_STATIC_DRAW);
   glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, this->ParPosBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->effectMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
   glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
   glEnableVertexAttribArray(1);

   glBindBuffer(GL_ARRAY_BUFFER, this->ParColBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->effectMaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
   glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);
   glEnableVertexAttribArray(2);

   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER,0);

}

void ParticleEffect::createFirstParticles(float coef){
   for(int i=0 ; i < (int)(this->effectMaxParticles*coef);i++) this->createSimpleParticle();
}

void ParticleEffect::createSimpleParticle(){
   this->effectParticlesLeft--;
   //Losowanie w prawie kuli; nie mam na razie pomysłu jak zrobić aby były to zmienne niezależne, więc wygląda jak jajko :/
   float posX = (float)(rand()%max( (int)ceil(this->effectMinSize*2000),1 )) / 1000 - this->effectMinSize;
   float maxy = sqrt(pow(this->effectMinSize,2)-pow(posX,2));
   float posY= (float) (rand()%max( (int)ceil(maxy*2000),1)) / 1000 - maxy;
   float maxz=this->effectMinSize - sqrt(pow(posX,2)+pow(posY,2));
   float posZ=(float)(rand()%max( (int)ceil(maxz*2000),1)) / 1000 -maxz;

   Particle p;
   p.colision=false;
   p.pos=glm::vec3(posX,posY,posZ);
   p.life=this->singleParticleLife*((float)(rand()%20+80)/90);
   int tmp = rand()%100;
   if(tmp < (this->effectMaxTime-this->effectTimeLeft)*(100/this->effectMaxTime) ) p.type=0;
   else p.type=1;

   float maxSpeed=(float)(this->effectMaxSize-this->effectMinSize)/this->singleParticleLife*2.8;
   //TODO Sensowniejsze losowanie
   float speedX=(float)(rand()%max( (int)ceil(maxSpeed*200),1))/100 -maxSpeed;
   float speedY=(float)(rand()%max( (int)ceil(maxSpeed*200),1))/100 -maxSpeed;
   float speedZ=(float)(rand()%max( (int)ceil(maxSpeed*200),1))/100 -maxSpeed;
   p.speed=glm::vec3(speedX,speedY,speedZ);

   p.size=(float)(rand()%100)/1000+0.08;
   this->particlesContainer.push_back(p);
}

//Calculate particles
void ParticleEffect::run(){
   this->effectTimeLeft-=inputActions::getInstance().deltaTime;

   this->effectMinSize += this->effectMaxSize/this->effectMaxTime*inputActions::getInstance().deltaTime;
   int iloscNowych = this->effectParticlesLeft/(this->singleParticleLife-1)*inputActions::getInstance().deltaTime;
   for(int i=0;i<iloscNowych;i++) this->createSimpleParticle();

   //main loop
   this->particlesCount=0;
   for(unsigned int i=0;i<this->particlesContainer.size();i++){
      Particle& p = this->particlesContainer[i];
      if(p.life>0){
         p.life-=inputActions::getInstance().deltaTime;

         this->effectParticlePosAndSize[4*particlesCount]=p.pos.x + this->effectPos.x;
         this->effectParticlePosAndSize[4*particlesCount+1]=p.pos.y + this->effectPos.y;
         this->effectParticlePosAndSize[4*particlesCount+2]=p.pos.z + this->effectPos.z;
         this->effectParticlePosAndSize[4*particlesCount+3]=p.size;

         if(this->effectParticlePosAndSize[4*particlesCount+1]-2 <= Map::getInstance().returnMapPointHeight(this->effectParticlePosAndSize[4*particlesCount],this->effectParticlePosAndSize[4*particlesCount+2]))
         p.colision=true;

         if(p.size>0.1) p.size-=0.05*inputActions::getInstance().deltaTime;
         if(!p.colision){
            p.speed.y-=9.81*inputActions::getInstance().deltaTime;
            p.speed.x*=0.995;
            p.speed.z*=0.995;
            p.pos+=p.speed*inputActions::getInstance().deltaTime; //pozycja
         }

         if( (p.type!=0) ){
            this->effectColorData[2*particlesCount] = p.life/this->singleParticleLife/2;
            if( p.speed.x > 0) this->effectColorData[2*particlesCount]=(this->effectColorData[2*particlesCount]*-1)+1;
            this->effectColorData[2*particlesCount+1]= p.life/this->singleParticleLife/2;
            if (p.speed.y > 0) this->effectColorData[2*particlesCount+1]=(this->effectColorData[2*particlesCount+1]*-1)+1;
         }
         else{
            this->effectColorData[2*particlesCount]= 0.05f;
            this->effectColorData[2*particlesCount+1]= 0.05f;
         }

         this->particlesCount++;
      }

   }
}
//draw particles
void ParticleEffect::draw(glm::mat4 projection, glm::mat4 modelView){
   this->shader->useShaderProgram(0);
   glBindVertexArray(this->VAO);

   glBindBuffer(GL_ARRAY_BUFFER, this->ParPosBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->effectMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, this->particlesCount * sizeof(GLfloat) * 4, this->effectParticlePosAndSize);

   glBindBuffer(GL_ARRAY_BUFFER, this->ParColBuffer);
   glBufferData(GL_ARRAY_BUFFER, this->effectMaxParticles * 2 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, this->particlesCount * sizeof(GLfloat) * 2, this->effectColorData);

   glUniform1i(glGetUniformLocation(this->shader->shaderProgram[0], "ourTexture1"), 0);
   glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
   GLint V = glGetUniformLocation(this->shader->shaderProgram[0], "V");
   GLint P = glGetUniformLocation(this->shader->shaderProgram[0], "P");
   glUniformMatrix4fv(V, 1, GL_FALSE, glm::value_ptr(modelView));
   glUniformMatrix4fv(P, 1, GL_FALSE, glm::value_ptr(projection));

   glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
   glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
   glVertexAttribDivisor(2, 1); // color : one per quad -> 1
   // This is equivalent to :
   // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
   // but faster.
   glDrawArraysInstanced(GL_TRIANGLES, 0, 6, this->particlesCount);
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER,0);
}
