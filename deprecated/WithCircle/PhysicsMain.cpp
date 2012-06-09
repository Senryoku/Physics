#include <cstdlib>
#include <iostream>

#include "Physics.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>

#ifdef SFML_SYSTEM_MACOS
	#include "ResourcePath.hpp"
#endif
/* Simple test de Physics.h */

using namespace Physics;

void glDrawCube(const sf::Vector2f &Position, float size);

GLuint glTexLoad(const char* Path)
{
	sf::Image image;
	bool LoadSuccess = 0;
	GLuint texture = 0;

	if(Path[0] != '\0')
	{
		#ifdef SFML_SYSTEM_MACOS
		LoadSuccess = image.loadFromFile(Path); //À rajouter ResourcePath selon compilation
		#else
		LoadSuccess = image.loadFromFile(Path);
		#endif
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if(LoadSuccess)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	} else {
		/* Texture transparente si le fichier n'a pas pu être chargé */
		GLubyte TexNull[4] =
		{
			0,0,0,0
		};
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, TexNull);
	}
	glDisable(GL_TEXTURE_2D);

	return texture;
}

int main(int argc, char** argv)
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	window.setVerticalSyncEnabled(1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 800.0, 600.0, 0.0, 0.0, 100.0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.f, 0.f, 1.f);

	sf::Clock vent;
	float forceVent(1.f);

	//Une texture OpenGL
	GLuint texture = glTexLoad("data/cute.png");

	Vertex* P4 = new Vertex();
	P4->setPosition(Vec2(100, 25));
	P4->setFixe();
	Vertex* P5 = new Vertex();
	P5->setPosition(Vec2(50, 50));
	Vertex* P6 = new Vertex();
	P6->setPosition(Vec2(100, 150));
	Vertex* P7 = new Vertex();
	P7->setPosition(Vec2(150, 50));
	Polygon* VP1 = new Polygon(4, WITH_LENGTH, P4, 100.f, P5, 100.f, P6, 100.f, P7, 100.f);
	new Rigid(P4, P6, sqrt(20000.f));
	new Rigid(P5, P7, sqrt(20000.f));

	Vertex* pLeftTop = new Vertex(), *pRightTop = new Vertex(), *pRightBottom = new Vertex(), *pLeftBottom = new Vertex();
	pLeftTop->setPosition(Vec2(300.f, 10.f));
	pRightTop->setPosition(Vec2(400.f, 10.f));
	pLeftBottom->setPosition(Vec2(300.f, 110.f));
	pRightBottom->setPosition(Vec2(400.f, 110.f));

	new Polygon(4, FLAG_NULL, pLeftTop, pRightTop, pRightBottom, pLeftBottom);
	new Rigid(pLeftTop, pRightBottom);// sqrt(20000.f));
	new Rigid(pLeftBottom, pRightTop);// sqrt(20000.f));

	float taille_cubes = 50.f;

	Vertex* P41 = new Vertex();
	P41->setPosition(Vec2(150, 150));
	P41->setFixe();
	Vertex* P42 = new Vertex();
	P42->setPosition(Vec2(250, 150));
	P42->setFixe();
	Vertex* P43 = new Vertex();
	P43->setPosition(Vec2(100, 300));
	P43->setFixe();

	new Polygon(3, FLAG_NULL, P41, P42, P43);

	//Vertex* P8 = new Vertex();

	/*
	//De l'eau?
	const int waterN(30); //MULTIPLE DE 2
	Vertex *pWater[waterN], *pWaterFixed[waterN];
	for (int i=0; i<waterN; i++)
	{
		pWater[i]= new Vertex(50.f+i*10.f, 300.f);
		pWater[i]->setMass(0.1f);
		if (i==0 || i==waterN-1)
			pWater[i]->setFixe();
		pWaterFixed[i]=new Vertex(50.f+i*10.f, 300.f);
		pWaterFixed[i]->setFixe();
		pWaterFixed[i]->setMass(3.f);
		new Elastic(pWaterFixed[i], pWater[i], 0.f, 0.75f);
		if (i>0)
			new Elastic(pWater[i], pWater[i-1]);
	}
	*/

	//Un petit rideau :D
	const int rows=20, colums=20, cTimes(2);
	bool cType(0);
	const float tailleCarre(20.f);
	Vertex* pRideau[rows*colums]={NULL};
	for (int i=0; i<colums; i++)
		for (int j=0; j<rows; j++)
		{
			pRideau[i+j*colums]=new Vertex();
			pRideau[i+j*colums]->setPosition(Vec2(300.f+i*tailleCarre, 30.f+j*tailleCarre));
			pRideau[i+j*colums]->setMass(.5f);
			//On fixe deux des points
			if ((i==0 && j==0) || (i==0  && j==rows-1))// || (i==colums-1 && j==0) || (i==colums-1  && j==rows-1))
				pRideau[i+j*colums]->setFixe();
			if (j!=0 && i==0)
				pRideau[i+j*colums]->setFixe();

			//On fait le lien avec celui qui est à gauche est en haut
			if (i>0) //on peut faire le lien sur la gauche
				for (int a=0; a<cTimes; a++)
				{
					if (cType) {
						new Elastic(pRideau[i+j*colums-1], pRideau[i+j*colums], -1.f, 10.f);
						// if(j > 0) new Elastic(pRideau[i+(j - 1)*colums-1], pRideau[i+j*colums], -1.f, 10.f);
					} else {
						new Rigid(pRideau[i+j*colums-1], pRideau[i+j*colums]);
						// if(j > 0) new Rigid(pRideau[i+(j - 1)*colums-1], pRideau[i+j*colums]);
						if(j > 0) new Elastic(pRideau[i+(j - 1)*colums-1], pRideau[i+j*colums], -1.f, 0.1f);
						if(j < rows-1) new Elastic(pRideau[i+(j + 1)*colums-1], pRideau[i+j*colums], -1.f, 0.1f);
					}
				}

			if (j>0) //on peut faire le lien sur la gauche
				for (int a=0; a<cTimes; a++)
				{
					if (cType)
						new Elastic(pRideau[i+(j-1)*colums], pRideau[i+j*colums], -1.f, 5.f);
					else
						new Rigid(pRideau[i+(j-1)*colums], pRideau[i+j*colums]);
				}

			if (i==1 && j==0)
				new Elastic(pRideau[i+j*colums-1], pRideau[i+j*colums], -1.f, 2.f);
			if (i==0 && j==1)
				new Elastic(pRideau[i+(j-1)*colums], pRideau[i+j*colums], -1.f, 2.f);

		}


	int i = 0;
	float prevdt = 0.1f, dt = 0.1f;

	Vertex *grab = NULL;
	Elastic* MouseElastic = NULL;
	Vertex* Mouse = new Vertex();

	Rectangle* rP;
	rP=new Rectangle(40.f, 60.f);
	rP->getTopLeft().setPosition(Vec2(10.f,400.f));
	//rP->setFixed();

	float polygon = 20.f;

	while(window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window : exit
			if (event.type == sf::Event::Closed)
				window.close();

			// Escape pressed : exit
			if (event.type == sf::Event::KeyPressed)
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::R:
                        (new Rectangle(50.f, 50.f))->getTopLeft().setPosition(Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
                        break;
                    case sf::Keyboard::P:
                         //new Polygon(8, FLAG_NULL, new Vertex(0.f, 1.f*polygon), new Vertex(1.f*polygon, 0.f), new Vertex(2.f*polygon, 0.f),
                                    // new Vertex(3.f*polygon, 1.f*polygon), new Vertex(3.f*polygon, 2.f*polygon), new Vertex(2.f*polygon, 3.f*polygon),
                                    // new Vertex(1.f*polygon, 3.f*polygon), new Vertex(0.f, 2.f*polygon));
                         new Polygon(6, FLAG_NULL, new Vertex(0.f, 1.f*polygon), new Vertex(1.f*polygon, 0.f),
                                     new Vertex(2.f*polygon, 1.f*polygon), new Vertex(2.f*polygon, 2.f*polygon),
                                     new Vertex(1.f*polygon, 3.f*polygon), new Vertex(0.f, 2.f*polygon));
                    default:
                        break;
                }


			// Adjust the viewport when the window is resized
            if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);

			if (event.type == sf::Event::MouseButtonPressed)
			{

				grab=Vertex::getNearest(Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
				if(event.mouseButton.button == sf::Mouse::Left)
					delete MouseElastic,
					MouseElastic = new Physics::Elastic(grab, Mouse, 1.f, 5.f);
				if(event.mouseButton.button == sf::Mouse::Middle || event.mouseButton.button == sf::Mouse::XButton1)
					(new Rectangle(25.f, 25.f))->getTopLeft().setPosition(Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
			}


		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && grab!=NULL)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
				grab->setPosition(Vec2(sf::Mouse::getPosition(window).x, grab->getPosition().y));
			else
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
					grab->setPosition(Vec2(grab->getPosition().x, sf::Mouse::getPosition(window).y));
				else
					grab->setPosition(Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				rP->applyForce(Vec2(50.f,0.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				rP->applyForce(Vec2(-50.f,0.f));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				rP->applyForce(Vec2(0.f,-50.f));

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				Mouse->setPosition(Vec2(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));
		//else if(MouseElastic != NULL) delete MouseElastic, MouseElastic = NULL;
		else Mouse->setFixe();

		glClear(GL_COLOR_BUFFER_BIT);

		//On prepare la projection
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); //On charge l'identite pour dessiner normalement
		glTranslatef(0.375, 0.375, 0.0); //petit trick qui assure le dessin à la  bonne place

		if (vent.getElapsedTime().asSeconds()>=4.f)
			forceVent=(forceVent>0 ? -1 : 1)*((rand()%400)/100.f+3.f), vent.restart();

		for(int i = 0; i < 1; i++)
        {
            //Physics::ForceAll(Vec2(forceVent, 0.f)); // Vent
            Physics::addAccelerationAll(Vec2(0.f, 12.f)); // Gravité
			Physics::Update(prevdt, dt);
			prevdt = dt; // Permet de gérer des framerate inconstants
        }

		//On affiche le rideau
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBegin(GL_QUADS);

		for (int i=0; i<colums; i++) //On fait de 4 en quatre ??
			for (int j=0; j<rows; j++)
			{
				if (i>=colums-1 || j>=rows-1) continue;

				//left top
				glTexCoord2f(static_cast<float>(i)/static_cast<float>(colums), static_cast<float>(j)/static_cast<float>(rows));
				glVertex2f(pRideau[i+j*colums]->getPosition().x, pRideau[i+j*colums]->getPosition().y);

				//right top
				glTexCoord2f(static_cast<float>(i+1)/static_cast<float>(colums), static_cast<float>(j)/static_cast<float>(rows));
				glVertex2f(pRideau[i+j*colums+1]->getPosition().x, pRideau[i+j*colums+1]->getPosition().y);

				//right bottom
				glTexCoord2f(static_cast<float>(i+1)/static_cast<float>(colums), static_cast<float>(j+1)/static_cast<float>(rows));
				glVertex2f(pRideau[i+(j+1)*colums+1]->getPosition().x, pRideau[i+(j+1)*colums+1]->getPosition().y);

				//Left bottom
				glTexCoord2f(static_cast<float>(i)/static_cast<float>(colums), static_cast<float>(j+1)/static_cast<float>(rows));
				glVertex2f(pRideau[i+(j+1)*colums]->getPosition().x, pRideau[i+(j+1)*colums]->getPosition().y);
		}

		glEnd();
		glDisable(GL_TEXTURE_2D);

		/*
		//on affiche l'eau
		glColor3f(0.f, 0.1f, 1.f);
		glBegin(GL_QUADS);
		for (int i=0; i<waterN-1; i++)
		{
			glVertex2f(pWaterFixed[i]->getPosition().x, pWaterFixed[i]->getPosition().y+200.f);
			glVertex2f(pWater[i]->getPosition().x, pWater[i]->getPosition().y);
			glVertex2f(pWater[i+1]->getPosition().x, pWater[i+1]->getPosition().y);
			glVertex2f(pWaterFixed[i+1]->getPosition().x, pWaterFixed[i+1]->getPosition().y+200.f);

		}
		glEnd();
		*/


		for (int i=0; i<10; i++)
		glDrawCube(sf::Vector2f(i*20.f, 400.f), 20.f);

		Vertex::DrawAll();
		Constraint::DrawAll();

		window.display();
	}

	Physics::DeleteAll();
	//On libere la texture
	glDeleteTextures(1, &texture);
}


void glDrawCube(const sf::Vector2f &Position, float size)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(Position.x, Position.y, 0.f);

	glBegin(GL_QUADS);

	//glColor3f(1.f, 0.f, 0.f);
	//front
	/*
	glTexCoord2f(0, 0); glVertex3f(0.f, 0.f, 0.f);
	glTexCoord2f(0, 1); glVertex3f(0.f,  size, 0.f);
	glTexCoord2f(1, 1); glVertex3f( size,  size, 0.f);
	glTexCoord2f(1, 0); glVertex3f( size, 0.f, 0.f);
	*/
	//back
	glTexCoord2f(0, 0); glVertex3f(0.f, 0.f, size);
	glTexCoord2f(0, 1); glVertex3f(0.f,  size, size);
	glTexCoord2f(1, 1); glVertex3f( size,  size, size);
	glTexCoord2f(1, 0); glVertex3f( size, 0.f, size);

	//left side
	glTexCoord2f(0, 0); glVertex3f(0.f, 0.f, 0.f);
	glTexCoord2f(0, 1); glVertex3f(0.f,  size, 0.f);
	glTexCoord2f(1, 1); glVertex3f(0.f,  size,  size);
	glTexCoord2f(1, 0); glVertex3f(0.f, 0.f,  size);

	//right side
	glTexCoord2f(0, 0); glVertex3f(size, 0.f, 0.f);
	glTexCoord2f(0, 1); glVertex3f(size,  size, 0.f);
	glTexCoord2f(1, 1); glVertex3f(size,  size,  size);
	glTexCoord2f(1, 0); glVertex3f(size, 0.f,  size);

	//Top
	glTexCoord2f(0, 1); glVertex3f(0.f, 0.f,  size);
	glTexCoord2f(0, 0); glVertex3f(0.f, 0.f, 0.f);
	glTexCoord2f(1, 0); glVertex3f( size, 0.f, 0.f);
	glTexCoord2f(1, 1); glVertex3f( size, 0.f,  size);

	//bottom
	glTexCoord2f(0, 1); glVertex3f(0.f, size,  size);
	glTexCoord2f(0, 0); glVertex3f(0.f, size, 0.f);
	glTexCoord2f(1, 0); glVertex3f( size, size, 0.f);
	glTexCoord2f(1, 1); glVertex3f( size, size,  size);

	glEnd();

	glPopMatrix();
}
