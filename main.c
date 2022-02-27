//Computer Seminar I Final Assignment - Joshua Teguh Santoso
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <GL/glfw.h>
#include <FTGL/ftgl.h>

#define CrossingNumber 100 //Reserve memory for crossing details
#define MaxName 50 //Reserve memory for crossing's name
#define PathNumber 100 //Reserve memory for path's array

int crossing_number; //Defining the global variable of number of crossings
int search_results[CrossingNumber]; //Defining the array for the results of the search
int n=0; //Defining the variable for loop of the search_results
int origin,finish; //Defining the variable of origin (to show the starting point on the map), start (the value of starting crossing), and finish (the value of final destination)
int path[PathNumber+1];//Defining the path array
int number,state=0,scrollstate=0;

double ORIGIN_X, ORIGIN_Y; //Defining the coordinate of the origins
double REAL_SIZE_X, REAL_SIZE_Y; //Defining the zoom value

typedef struct //Defining structure for position
{
	double x,y;
} position;

typedef struct //Defining structure for crossing details
{
	int id;
	char name[MaxName];
	position pos;
	double distance;
	double wait;
	double wait_time;
	int points;
	int next[5];
	int previous;
	int visited;
} crossing;

crossing cross[CrossingNumber];

void map_read(char *filename) //Defining function to read the crossing details from external file
{
	int i, j;
	FILE *fp;
	fp = fopen(filename,"r");
	if(fp == NULL)
	{
		printf(" *** file is not created *** \n");
		return 0;
	}

	fscanf(fp, "%d", &crossing_number);
	if((crossing_number<1) || (crossing_number>=CrossingNumber))
	{
		printf("Illegal data number (%d)\n", crossing_number);
		return 0;
	}

	for(i=0;i<crossing_number;i++)
	{
		//Scanning the crossing's details into the structure
		fscanf(fp,"%d,%lf,%lf,%lf,%[^,],%d", &(cross[i].id), &(cross[i].pos.x), &(cross[i].pos.y),&(cross[i].wait), cross[i].name, &(cross[i].points));
		for(j=0;j<cross[i].points;j++) 
		{
			fscanf(fp,",%d",&(cross[i].next[j]));
		}
	}
	fclose(fp);
}

void line(double x0, double y0, double x1, double y1) //Defining the function to draw a line
{
	glBegin(GL_LINES);
	glVertex2d(x0,y0);
	glVertex2d(x1,y1);
	glEnd();
}

void circle(double x, double y, double r) //Defining the function to draw a circle
{
	int const N = 24;
	int i;

	glBegin(GL_LINE_LOOP);
	for(i=0;i<N;i++)
	{
		glVertex2d(x + cos(2* M_PI * i / N) * r, y +sin(2 * M_PI * i / N) * r);
	}
	glEnd();
}

void circle_r(double x, double y, double r) //Defining the function to draw a red circle
{
	int const N = 24;
	int i;

	glBegin(GL_POLYGON);
	for(i=0;i<N;i++)
	{
		glVertex2d(x + cos(2* M_PI * i / N) * r, y +sin(2 * M_PI * i / N) * r);
	}
	glEnd();
}

void vehicle(double x, double y, double r) //Defining the function to draw the vehicle in the shape of a circle
{
	int const N = 24;
	int i;

	glBegin(GL_POLYGON);
	for(i=0;i<N;i++)
	{
		glVertex2d(x + cos(2* M_PI * i / N) * r, y +sin(2 * M_PI * i / N) * r);
	}
	glEnd();
}

#ifndef FONT_FILENAME
#define FONT_FILENAME "Roboto-Black.ttf"
#endif
FTGLfont *font;

void setupfont() //Function to set up the font
{
	font = ftglCreateExtrudeFont(FONT_FILENAME);
	if(font == NULL)
	{
		perror(FONT_FILENAME);
		fprintf(stderr, "could not load font\n");
		exit(1);
	}
	ftglSetFontFaceSize(font,24,24);
	ftglSetFontDepth(font,0.01);
	ftglSetFontOutset(font,0,0.1);
	ftglSetFontCharMap(font,ft_encoding_unicode);
}

void outtextxy(double x, double y,double deg, char const *text) //Function to display text in OpenGL (glfw)
{
	double const scale=0.01;
	glPushMatrix();
	glTranslated(x,y,0.0);
	glRotatef(-deg,0,0,1);
	glScaled(scale,scale,scale);
	ftglRenderFont(font,text,FTGL_RENDER_ALL);
	glPopMatrix();
}

//Defining some of the keys of the keyboard
#define 	GLFW_KEY_A   65
#define 	GLFW_KEY_D   68
#define 	GLFW_KEY_E   69
#define 	GLFW_KEY_Q   81
#define 	GLFW_KEY_R   82
#define 	GLFW_KEY_S   83
#define 	GLFW_KEY_W   87

void key_callback(int key,int action) //Function to change the state of something by pressing a certain key
{
	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		state=1;
	}
	else if(key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		state=2;
	}
	else if(key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		state=3;
	}
	else if(GLFW_KEY_A && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		state=4;
	}
	else if(GLFW_KEY_D && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		state=5;
	}
	else if(GLFW_KEY_S && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		state=6;
	}
	else if(GLFW_KEY_W && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		state=7;
	}
	else if(key == GLFW_KEY_Q && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		state=8;
	}
	else if(key == GLFW_KEY_E && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		state=9;
	}
	else if(key == GLFW_KEY_UP && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		state=10;
	}
	else if(key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		state=0;
	}

	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		state=12;
	}
	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		state=13;
	}
}

int x,xpos,y,ypos;
void mouse() //Defining the mouse position
{
	glfwGetMousePos(&x,&y);
	xpos=x-360;
	ypos=360-y;
}


void swap(int *x, int i, int j) //Defining a swap function
{
	int temp = x[i];
	x[i] = x[j];
	x[j] = temp;
}

int dijkstra(int origin, int finish) //Defining the dijkstra algorithm function
{
	/*
	{
		In dijkstra algorithm, we check the distance value of each points and the set of points
		with the smallest value of the distance from origin to the destination is chosen as the path. 
	}
	*/
	int i,next,s,last=0;
	double dist,distance = 1e100; //Initializing the distance value
	s = origin;

	for(i=0;i<crossing_number;i++) //Initializing each points distance and visited(whether the point has been checked or not)
	{
		cross[i].distance = 1e100;
		cross[i].visited = 0;
	}
	for(i=0;i<PathNumber+1;i++) //Initializing the path array
	{
		path[i]=-1;
	}
	cross[origin].distance = 0;//Initializing the origin distance as 0
	while (last != 1) //Loop for when the 'last' flag is not 1
	{
		distance = 1e100;
		next=-1;
		for(i=0;i<cross[s].points;i++)
		{
			if(cross[cross[s].next[i]].visited == 0)
			{
				dist=(hypot(cross[cross[s].next[i]].pos.x-cross[s].pos.x,cross[cross[s].next[i]].pos.y-cross[s].pos.y))+cross[s].distance;
				if(dist<cross[cross[s].next[i]].distance)
				{
					cross[cross[s].next[i]].distance = dist;
					cross[cross[s].next[i]].previous = s;
				}
			}
		}
		cross[s].visited = 1;//changing the visited value of the points
		if(s==finish)
		{
			last = 1; //changing the value of 'last' flag if the next point is the destination one
			continue;
		}
		for(i=0;i<crossing_number;i++)
		{
			if(distance>cross[i].distance && cross[i].visited == 0)//checking the points with the smallest distance value and is not visited
			{
				next = i;
				distance = cross[i].distance;
			}
		}
		s=next;
	}
	if(last ==  1) //When the 'last' flag is 1
	{
		path[0]=s;
		for(i=1;s!=origin;i++)
		{
			path[i]=cross[s].previous;
			s = cross[s].previous;
			number = i;
		}
		/*
		{
			Since the path array is reversed like path[0] = destination and path[number] = origin
			so we can do it 2 ways which one of them is done by swapping the values
		}
		*/
		if(number % 2 ==0) //Swapping for even number of the number of paths
		{
			for(i=0;(number-i)!=(number/2);i++)
			{
				swap(path,i,number-i);
			}
		}
		else //Swapping for odd number of the number of paths
		{
			for(i=0;(number-i)!=(floor(number/2));i++)
			{
				swap(path,i,number-i);
			}
		}
	}
	return 0;
}

int draw_map(int search_directions) //Defining the function to draw the map
{
	int i,j,start_zooming=0,start_moving=0;
	int selected=-1,screensize_x,screensize_y;
	int width=720,height=720;
	int vehicle_pathIterator,vehicle_stepOnEdge;
	double vehicle_x = 0.0, vehicle_y = 0.0, x0, y0, x1, y1, deg=0,deg1=0,deg_z=0;

	glfwInit();
	glfwOpenWindow(width,height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);

	setupfont();
	if(search_directions==1)
	{
		vehicle_pathIterator=dijkstra(origin,finish); //Calling the the dijkstra function
		vehicle_stepOnEdge=0;
	}

	REAL_SIZE_X = 20.0, REAL_SIZE_Y = 20.0;

	for(;;)
	{
		if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
		break;

		//Conditionals for the rotation when the vehicle is moving
		if(deg-deg1<-180)
		{
			deg1 += 2;
			if(deg-deg1<-360)
			{
				deg1=deg;
			}
		}
		else if(deg-deg1<0)
		{
			deg1 -=2;
			if(deg-deg1>0)
			{
				deg1=deg;
			}
		}
		if(deg-deg1>180)
		{
			deg1 -= 2;
			if(deg-deg1>360)
			{
				deg1=deg;
			}
		}
		else if(deg-deg1>0)
		{
			deg1 += 2;
			if(deg-deg1<0)
			{
				deg1=deg;
			}
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glRotatef(deg1,0,0,1);
		glRotatef(deg_z,1,0,0);
		glOrtho(ORIGIN_X + REAL_SIZE_X * -0.5, ORIGIN_X + REAL_SIZE_X * 0.5,
			ORIGIN_Y + REAL_SIZE_Y * -0.5, ORIGIN_Y + REAL_SIZE_Y * 0.5
			,-100.0,100.0);

		glfwGetWindowSize(&width, &height);
		glViewport(0,0,width,height);
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		double p_x,p_y;
		
		//Calling the keyboard keys
		glfwSetKeyCallback(key_callback);
		//Conditionals of the condition when a certain key is pressed
		if(state == 1)
		{
			REAL_SIZE_X -=0.25;
			REAL_SIZE_Y -=0.25;
		}
		else if(state == 2)
		{
			REAL_SIZE_X +=0.25;
			REAL_SIZE_Y +=0.25;
		}
		else if(state == 3)
		{
			ORIGIN_X -= 0.25;
		}
		else if(state == 4)
		{
			ORIGIN_X += 0.25;
		}
		else if(state == 5)
		{
			ORIGIN_Y -= 0.25;
		}
		else if(state == 6)
		{
			ORIGIN_Y += 0.25;
		}
		else if(state == 7)
		{
			deg -= 5;
		}
		else if(state == 8)
		{
			deg += 5;
		}
		else if(state==9)
		{
			deg_z +=5;
		}
		else if (state==10)
		{
			deg_z -=5;
		}
		else if(state==13)
		{
			ORIGIN_X = 0;
			ORIGIN_Y = 0;
		}

		//Conditionals for scrolling 
		int temp;
		int zoomstate = glfwGetMouseWheel();
		if(zoomstate-temp>0)
		{
			REAL_SIZE_X -= 3.0;
			REAL_SIZE_Y -= 3.0;
		}
		else if(zoomstate-temp<0)
		{
			REAL_SIZE_X += 3.0;
			REAL_SIZE_Y += 3.0;
		}
		temp = zoomstate;

		//Calling the mouse function in order to use mouse input
		int action=glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
		mouse();

		//When the user wants the direction 
		if(search_directions==1)
		{
			if(start_zooming==0)
			{	
				glColor3d(0.5,0.5,0.5);
				outtextxy(5,-6,0,"Press SPACE to start moving."); //Prompting to start the vehicle to move
				if(state==12)
				{
					start_zooming=1;
				}
			}
			else if(start_zooming==1) //Automatic zoom
			{	
				if(vehicle_x == 0 && vehicle_y ==0 && start_moving == 0)
				{
					ORIGIN_X = cross[origin].pos.x, ORIGIN_Y = cross[origin].pos.y;
				}
				else
				{
					ORIGIN_X = vehicle_x, ORIGIN_Y = vehicle_y;
				}
				if(REAL_SIZE_X > 8.0 && REAL_SIZE_Y > 8.0)
				{
					REAL_SIZE_X -= 0.25;
					REAL_SIZE_Y -= 0.25;
				}
				else
				{
					REAL_SIZE_X = 8.0, REAL_SIZE_Y = 8.0;
					start_moving=1;
				}
			}

		}
		//When the user wants to search only (automatic zoom)
		else if(search_directions==0)
		{
			if(n>1)
			{	
				ORIGIN_X = 0.0, ORIGIN_Y = 0.0;
			}
			else if(n==1)
			{
				ORIGIN_X = cross[search_results[0]].pos.x, ORIGIN_Y = cross[search_results[0]].pos.y;
				if(REAL_SIZE_X > 8.0 && REAL_SIZE_Y > 8.0)
				{
					REAL_SIZE_X -= 0.25;
					REAL_SIZE_Y -= 0.25;
				}
				else
				{
					REAL_SIZE_X = 8.0, REAL_SIZE_Y = 8.0;
				}
			}
		}
		//When the user wants to find the position of the map by showing the map and click to show what crossing it is
		if(search_directions==3)
		{
			for(i=0;i<CrossingNumber;i++)
			{
				if(cross[i].pos.x<0)
				{
					p_x=-0.5;
					screensize_x=-360;
				}
				else if(cross[i].pos.x>=0)
				{
					p_x=0.5;
					screensize_x=360;
				}
				if(cross[i].pos.y<0)
				{
					p_y=-0.5;
					screensize_y=-360;
				}
				else if(cross[i].pos.y>=0)
				{
					p_y=0.5;
					screensize_y=360;
				}
			}
			for(i=0;i<CrossingNumber;i++)
			{
				//Conditional of when the button is activated by the mouse position
				if(xpos<((cross[i].pos.x*screensize_x)/(ORIGIN_X+REAL_SIZE_X*p_x))+((0.1*screensize_x)/(REAL_SIZE_X*p_x))
					&& xpos>((cross[i].pos.x*screensize_x)/(ORIGIN_X+REAL_SIZE_X*p_x))-((0.1*screensize_x)/(REAL_SIZE_X*p_x))
					&& ypos<((cross[i].pos.y*screensize_y)/(ORIGIN_Y+REAL_SIZE_Y*p_y))+((0.1*screensize_y)/(REAL_SIZE_Y*p_y))
					&& ypos>((cross[i].pos.y*screensize_y)/(ORIGIN_Y+REAL_SIZE_Y*p_y))-((0.1*screensize_y)/(REAL_SIZE_Y*p_y))
					&& action==GLFW_PRESS)
				{
					selected = i;
					state=11;
				}
			}
			
			if(state==11)
			{
				//Displaying the selected crossing
				glColor3d(1.0,0.0,0.0);
				circle_r((cross[selected].pos.x),(cross[selected].pos.y),0.1);
				glColor3d(1.0,1.0,0.0);
				outtextxy((cross[selected].pos.x),(cross[selected].pos.y),0,(cross[selected].name));
			}
			
		}	
		glColor3d(1.0,1.0,1.0);
		//Drawing each crossings coordinates as points
		for(i=0;i<crossing_number;i++)
		{
			circle(cross[i].pos.x,cross[i].pos.y,0.1);
		}

		glColor3d(1.0,1.0,1.0);
		//Draw the line between each crossings (in accordance to the data of the crossings)
		for(i=0;i<crossing_number;i++)
		{
			for(j=0;j<cross[i].points;j++)
			{
				line((cross[i].pos.x),(cross[i].pos.y),(cross[(cross[i].next[j])].pos.x),(cross[(cross[i].next[j])].pos.y));
			}
		}

		glColor3d(0.0,0.0,1.0);
		if(search_directions==1) //Printing the path line of the direction
		{
			for(i=0;i<number;i++)
			{
				line(cross[path[i]].pos.x,cross[path[i]].pos.y,cross[path[i+1]].pos.x,cross[path[i+1]].pos.y);
			}
		}	

		glColor3d(1.0,0.0,0.0);
		//Drawing the start and finish location as red circle
		if(search_directions==1)
		{
			circle_r((cross[origin].pos.x),(cross[origin].pos.y),0.1);
			circle_r((cross[finish].pos.x),(cross[finish].pos.y),0.1);
		}
		else if(search_directions==0)
		{
			if(n==1)
			{
				circle_r((cross[(search_results[0])].pos.x),(cross[(search_results[0])].pos.y),0.1);
			}
			else if(n>1)
			{
				for(i=0;i<n;i++)
				{
					circle_r((cross[(search_results[i])].pos.x),(cross[(search_results[i])].pos.y),0.1);
				}
			}
		}

		glColor3d(1.0,1.0,0.0);
		if(search_directions==1)
		{	
			//Displaying the name of the crossing of the start and finish location
			outtextxy((cross[origin].pos.x),(cross[origin].pos.y),deg1,cross[origin].name);
			outtextxy((cross[finish].pos.x),(cross[finish].pos.y),deg1,cross[finish].name);
		}
		else if(search_directions==0)//Displaying the name of the crossing from the search results
		{
			if(n==1)
			{
				outtextxy((cross[(search_results[0])].pos.x),(cross[(search_results[0])].pos.y),deg,cross[search_results[0]].name);
			}
			else if(n>1)
			{
				for(i=0;i<n;i++)
				{
					outtextxy((cross[(search_results[i])].pos.x),(cross[(search_results[i])].pos.y),deg,cross[search_results[i]].name);
				}
			}
		}

		if(search_directions==1)
		{
			if(start_moving == 1)
			{
				if(path[vehicle_pathIterator+0] != -1 && path[vehicle_pathIterator+1] != -1)
				{
					x0 = cross[path[vehicle_pathIterator+0]].pos.x;
					y0 = cross[path[vehicle_pathIterator+0]].pos.y;
					x1 = cross[path[vehicle_pathIterator+1]].pos.x;
					y1 = cross[path[vehicle_pathIterator+1]].pos.y;	

					double distance = hypot(x1-x0,y1-y0);	
					deg = atan2((x1-x0),(y1-y0))*(180/M_PI);
					int steps = (int)(distance*50);
					vehicle_stepOnEdge++;
					vehicle_x = x0 + (x1-x0)/steps*vehicle_stepOnEdge;
					vehicle_y = y0 + (y1-y0)/steps*vehicle_stepOnEdge;

					glColor3d(1.0,1.0,0.0);
					outtextxy(cross[path[vehicle_pathIterator]].pos.x,cross[path[vehicle_pathIterator]].pos.y,deg1,cross[path[vehicle_pathIterator]].name);
					outtextxy(cross[path[vehicle_pathIterator+1]].pos.x,cross[path[vehicle_pathIterator+1]].pos.y,deg1,cross[path[vehicle_pathIterator+1]].name);

					if(vehicle_stepOnEdge>=steps)
					{
						vehicle_pathIterator++;
						vehicle_stepOnEdge=0;
					}
				
				}
				
				//Displaying the vehicle movement
				glColor3d(0.0,0.0,1.0);
				vehicle(vehicle_x,vehicle_y,0.1);
				if(vehicle_x == cross[finish].pos.x && vehicle_y == cross[finish].pos.y)
				{
					glColor3d(0.0,1.0,0.0);
					outtextxy(cross[finish].pos.x,cross[finish].pos.y+2,deg1,"You have arrived!");
				}	
			}
		}
		glfwSwapBuffers();
		usleep(30*1000);

	}

	glfwTerminate();

	return 0;
}

void search_name(int num_cross,int startfinish, int search_directions) //Defining a function to search by name
{
	int i,j,showinput=0,IDinput; 
	int id_break,search_break,searchbreak,YesNobreak;
	char buff[256],usrinput[10],usrinput1[10],YesNo;
	int f=0,l=0;

	for(i=0;i<crossing_number;i++)
	{
		search_results[i]=0; //Initializing the search results array
	}
	while(searchbreak!=1) //Loop to repeat the search if the user is not satisfied
	{
		search_break=0;
		while(search_break != 1) //Loop to avoid input error.
		{
			printf("Input the name of the crossing -->");
			scanf(" %[^\n]s",buff);

			for(i=0;i<num_cross;i++)
			{
				if(strcasestr(cross[i].name,buff)!=NULL) //stcasestr is used to compare the strings loosely with case insensitive. I do not use fnmatch because most of the users would not understand wild-card matching.
				{
					search_break=1;
					printf("Crossing ID:%d,Coordinates(%.2lf,%.2lf),Name:%s,Distance from origin:%.2lf ",cross[i].id, cross[i].pos.x, cross[i].pos.y, cross[i].name,hypot(cross[i].pos.x,cross[i].pos.y));
					printf("Wait time:%.2lf,points:%d,adjacent crossing(", cross[i].wait,cross[i].points);
					for(j=0;j<cross[i].points;j++)
					{
						printf("%d ",cross[i].next[j]);
					}	

					printf(")\n");
					f=1;
					search_results[n]=i; //Putting the serach result(s) into an array
					n = n + 1; //Increasing the n by 1.
				}
			}
			if(f==0)
			{
				printf("Could not find the crossing. Please try again\n");
			}
		}
		if(search_directions==1)
		{
			if(n>1)
			{
				id_break=0;
				while(id_break!=1) //Loop to avoid input error.
				{
					IDinput=-1;
					fgetc(stdin);
					while(IDinput==-1) //Loop to avoid input error.
					{
						printf("Please select one ID from the search results-->"); //Prompting the user to enter the crossing ID from the results shown
						fgets(usrinput,9, stdin);

						if(strlen(usrinput)< 2 ||  strlen(usrinput)> 3)
						{
							printf(" --- Input error. Please try again. --- \n");
							continue;
						}
						if(sscanf(usrinput, "%d", &IDinput)!=1)
						{
							IDinput=-1;
							printf(" --- Input error. Please try again. --- \n");
							continue;
						}
						if(IDinput<0 || IDinput>crossing_number)
						{
							IDinput=-1;
							printf(" --- Could not find crossing. Please try again. --- \n");
							continue;
						}
						
					}
					for(i=0;i<n;i++)
					{
						if(IDinput == search_results[i] && IDinput > 0)
						{					
							search_results[0] = IDinput; //Take the result that match with the user's input
							for(i=1;i<=n;i++)
							{
								search_results[i] =0; //Deleting other results from the array
							}
							l = 1;
							id_break=1;
						}
					}
				
					if(l==0)
					{				
						printf(" --- Input error. Please try again. --- \nPlease type ENTER to continue.\n"); //In case of user put ID other that what have been shown before
					}
				}
			i=search_results[0];
			printf("Crossing ID:%d,Coordinates(%.2lf,%.2lf),Name:%s,Distance from origin:%.2lf ",cross[i].id, cross[i].pos.x, cross[i].pos.y, cross[i].name,hypot(cross[i].pos.x,cross[i].pos.y));
			printf("Wait time:%.2lf,points:%d,adjacent crossing(", cross[i].wait,cross[i].points);
			for(j=0;j<cross[i].points;j++)
			{
				printf("%d ",cross[i].next[j]);
			}	
			printf(")\n");
			}
		}
		else if(search_directions==0)
		{
			if(n>1)
			{
				while(showinput==0) //Loop to avoid input error.
				{
					printf("Do you want to 1.Show all 2.Show only one?-->");
					fgetc(stdin);
					fgets(usrinput,9, stdin);	

					if(strlen(usrinput)< 2 ||  strlen(usrinput)> 3)
					{
						printf(" --- Input error. Please try again. --- \n");
						continue;
					}
					if(sscanf(usrinput, "%d", &showinput)!=1)
					{
						showinput=0;
						printf(" --- Input error. Please try again. --- \n");
						continue;
					}
					if(showinput<1 || showinput>2)
					{
						showinput=0;
						printf(" --- Input error. Please try again. --- \n");
						continue;
					}
				}
				printf("%d\n",showinput);
				if(showinput==2)
				{
					id_break=0;
					while(id_break!=1)//Loop to avoid input error.
					{
						IDinput=-1;
						while(IDinput==-1) //Loop to avoid input error.
						{
							printf("Please select one ID from the search results-->"); //Prompting the user to enter the crossing ID from the results shown
							fgets(usrinput1,9, stdin);
						
							if(strlen(usrinput1)< 2 ||  strlen(usrinput)> 3)
							{
								printf(" --- Input error. Please try again. --- \n");
								continue;
							}
							if(sscanf(usrinput1, "%d", &IDinput)!=1)
							{
								IDinput=-1;
								printf(" --- Input error. Please try again. --- \n");
								continue;
							}
							if(IDinput<0 || IDinput>crossing_number)
							{
								IDinput=-1;
								printf(" --- Could not find crossing. Please try again. --- \n");
								continue;
							}
						
						}
						for(i=0;i<n;i++)
						{
							if(IDinput == search_results[i] && IDinput > 0)
							{					
								search_results[0] = IDinput; //Take the result that match with the user's input
								for(i=1;i<=n;i++)
								{
									search_results[i] =0; //Deleting other results from the array
								}
								l = 1;
								n = 1;
								id_break=1;
							}
						}
				
						if(l==0)
						{				
							printf(" --- Input error. Please try again. --- \n"); //In case of user put ID other that what have been shown before
						}
					}
					i=search_results[0];
					printf("Crossing ID:%d,Coordinates(%.2lf,%.2lf),Name:%s,Distance from origin:%.2lf ",cross[i].id, cross[i].pos.x, cross[i].pos.y, cross[i].name,hypot(cross[i].pos.x,cross[i].pos.y));
					printf("Wait time:%.2lf,points:%d,adjacent crossing(", cross[i].wait,cross[i].points);
					for(j=0;j<cross[i].points;j++)
					{
						printf("%d ",cross[i].next[j]);
					}	
					printf(")\n");
				}
			}	
		}
		YesNobreak=0;
		while(YesNobreak!=1)//Loop to avoid input error
		{
			if(showinput==1)
			{
				printf("Are you sure? (y/n) -->");
				scanf(" %c",&YesNo);
			}
			else
			{
				printf("Is this what you are looking for? (y/n) -->"); //Prompting the user whether the result given is what the user wants
				scanf(" %c", &YesNo);
			}

			if(YesNo == 'y' && isalpha(YesNo))
			{
				YesNobreak=1;
				if(search_directions==1)
				{
					if(startfinish == 0) //If the startfinish is 0, it is representing starting position
					{
						origin=search_results[0];
					}
					else if(startfinish ==1) //If the startfinish is 1, it is representing final destination
					{
						finish=search_results[0];
					}
					searchbreak=1;
				}
				else if(search_directions==0)
				{
					searchbreak=1;
				}
			}
			else if(YesNo =='n' && isalpha(YesNo))
			{
				YesNobreak=1;
				searchbreak=0;
			}
			else
			{
				printf("--- Input error. Please try again. ---\n");
			}
		}
	}
}

void search_min_distance(int num_cross,int startfinish, int search_directions) //Defining a functon to search by minimum distance from the coordinates inputted
{
	int i;
	int search_break,YesNobreak;
	int minindex=0;
	double dist,mdist;
	double x,y;
	char YesNo;

	search_results[0]=0; //This function only gives out one result so initialising 1 data is sufficient
	
	while(search_break != 1) //Loop to end the search
	{ 	
		printf("Input the coordinates for a specific location (%%lf %%lf format) -->");
		scanf(" %lf %lf", &x, &y);
			
		mdist=hypot(cross[0].pos.x - x, cross[0].pos.y - y); //Initialising and calculate the minimum distance to origin

		for(i=0;i<num_cross;i++)
		{
			dist=hypot(cross[i].pos.x - x, cross[i].pos.y - y); //Calculating the distance of each point to the coordinates inputted
			if(dist<mdist) //If the distance calculated is less than the minimum distance then the minimum distance is changed
			{
				minindex = i; //To show which crossing has the minimum distance
				mdist=dist; 
			}
		}
		search_results[0]=cross[minindex].id;
		printf("Crossing closest to (%.2lf, %.2lf) is %s (Distance: %5.2lf)\n",x,y,cross[minindex].name,mdist);
		YesNobreak=0;
		while(YesNobreak!=1)
		{
			printf("Is this what you are looking for? (y/n) -->"); //Prompting the user whether the result given is what the user wants
			scanf(" %c", &YesNo);
			if(YesNo == 'y' && isalpha(YesNo))
			{
				YesNobreak=1;
				if(search_directions==1)
				{
					if(startfinish==0)
					{
						origin=search_results[0];
					}
					else if(startfinish==1)
					{
						finish=search_results[0];
					}
					search_break=1;
				}
				else if(search_directions==0)
				{
					n=1;
					search_break=1;
				}
			}
			else if(YesNo =='n' && isalpha(YesNo))
			{
				YesNobreak=1;
				search_break=0;
			}
			else
			{
				printf("--- Input error. Please try again. ---\n");
			}
		}
	}
}

void search_ID(int num_cross,int startfinish, int search_directions) //Defining a function to search by ID
{
	int i,k;
	int search_break=0;
	int num,YesNobreak;
	char YesNo,usrinput[10];

	search_results[0]=0; //This function only gives out one result so initialising 1 data is sufficient
	
	while(search_break != 1) //Loop to end the search
	{ 	
		num=-1;
		while(num==-1) //Loop to avoid input error
		{
			printf("Input the crossing ID -->");
			fgets(usrinput,9, stdin);

			if(strlen(usrinput)< 2 ||  strlen(usrinput)> 3)
			{
				printf(" --- Input error. Please try again. --- \n");
				continue;
			}
			if(sscanf(usrinput, " %d", &num)!=1)
			{
				num=-1;
				printf(" --- Input error. Please try again. --- \n");
				continue;
			}
			if(num<0 || num>crossing_number)
			{
				num=-1;
				printf(" --- Could not find crossing. Please try again. --- \n");
				continue;
			}
		}
		for(i=0;i<num_cross;i++)
		{
			if(num == cross[i].id)
			{
				printf("Crossing ID:%d,Coordinates(%.2lf,%.2lf),Name:%s,Distance from origin:%.2lf ",cross[num].id, cross[num].pos.x, cross[num].pos.y, cross[num].name,hypot(cross[num].pos.x,cross[num].pos.y));
				printf("Wait time:%.2lf,points:%d,adjacent crossing(", cross[num].wait,cross[num].points);
				for(k=0;k<cross[num].points;k++)
				{
					printf("%d ",cross[num].next[k]);
				}	
				printf(")\n");
				search_results[0]=i;
			}
		}
		YesNobreak=0;
		while(YesNobreak!=1) //Loop to avoid input error
		{
			printf("Is this what you are looking for? (y/n) -->"); //Prompting the user whether the result given is what the user wants
			scanf(" %c", &YesNo);
			if(YesNo == 'y' && isalpha(YesNo))
			{
				YesNobreak=1;
				if(search_directions==1)
				{
					if(startfinish==0)
					{
						origin=search_results[0];
					}
					else if(startfinish==1)
					{
						finish=search_results[0];
					}
					search_break=1;
				}
				else if(search_directions==0)
				{
					n=1;
					search_break=1;
				}
			}
			else if(YesNo =='n' && isalpha(YesNo))
			{
				YesNobreak=1;
				search_break=0;
			}
			else
			{
				printf("--- Input error. Please try again. ---\n");
			}
		}
	}
}

void search()
{
	int num;
	char usrinput[10];
	printf("Please select one of the search method below\n");
	while(num==0) //Loop to avoid input error
	{
		printf("1.Search by name\n2.Search by minimum distance\n3.Search by ID\n-->"); //Prompting the user to choose which search type he/she wants to use
		fgets(usrinput,9, stdin);

		if(strlen(usrinput)< 2 ||  strlen(usrinput)> 2)
		{
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(sscanf(usrinput, " %d", &num)!=1)
		{
			num=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(num<1 || num>3)
		{
			num=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
	}
	if(num == 1)
	{
		search_name(crossing_number,0,0); //Calling the search_name function 
	}
	else if(num == 2)
	{
		search_min_distance(crossing_number,0,0); //Calling the search_min_distance function
	}
	else if(num == 3)
	{
		search_ID(crossing_number,0,0); //Calling the search_ID
	}
	draw_map(0);
}

void direction()
{
	int num1=0,num2=0;
	char  usrinput[10],usrinput1[10];

	printf("Please input your starting point by selecting one of the search method below (Please insert a number)\n");
	while(num1==0) //Loop to avoid input error
	{ 
		printf("1.Search by name\n2.Search by minimum distance\n3.Search by ID\n-->"); //Prompting the user to choose which search type he/she wants to use
		fgets(usrinput,9, stdin);

		if(strlen(usrinput)< 2 ||  strlen(usrinput)> 2)
		{
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(sscanf(usrinput, " %d", &num1)!=1)
		{
			num1=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(num1<1 || num1>3)
		{
			num1=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
	}
	if(num1 == 1)
	{
		search_name(crossing_number,0,1); //Calling the search_name function 
	}
	else if(num1 == 2)
	{
		search_min_distance(crossing_number,0,1); //Calling the search_min_distance function
	}
	else if(num1 == 3)
	{
		search_ID(crossing_number,0,1); //Calling the search_ID
	}
	
	printf("Please input your destination, select one of the search method below (Please insert a number)\n");
	while(num2==0) //Loop to avoid input error
	{
		printf("1.Search by name\n2.Search by minimum distance\n3.Search by ID\n--->"); //Prompting the user to choose which search type he/she wants to use
		fgetc(stdin);
		fgets(usrinput1,9, stdin);

		if(strlen(usrinput1)< 2 ||  strlen(usrinput1)> 2)
		{
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(sscanf(usrinput1, " %d", &num2)!=1)
		{
			num2=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(num2<1 || num2>3)
		{
			num2=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
	}
	if(num2 == 1)
	{
		search_name(crossing_number,1,1); //Calling the search_name function 
	}
	else if(num2 == 2)
	{
		search_min_distance(crossing_number,1,1); //Calling the search_min_distance function
	}
	else if(num2 == 3)
	{
		search_ID(crossing_number,1,1); //Calling the search_ID
	}
	
	draw_map(1);
}
int main(void)
{
	int num=0;
	char search_or_directions[10];

	map_read("map.dat"); //Calling the map_read function to read the crossing's detail from external data
	printf("---WELCOME TO PATHFINDER ver 1.ULTIMATE by Joshua Teguh Santoso---\n");
	while(num==0) //Loop to avoid input error
	{
		printf("How can I help you?\n1.Show Map\n2.Search Crossings\n3.Finding Directions\n(Please input the number)\n-->");
		fgets(search_or_directions,9, stdin);

		if(strlen(search_or_directions)< 2 ||  strlen(search_or_directions)> 2)
		{
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(sscanf(search_or_directions, " %d", &num)!=1)
		{
			num=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
		if(num<1 || num>3)
		{
			num=0;
			printf(" --- Input error. Please try again. --- \n");
			continue;
		}
	}
		if(num==1)
		{
			draw_map(3);
		}
		else if(num==2)
		{	
			search();
		}
		else if(num==3)
		{
			direction();
		}
	return 0;
}
