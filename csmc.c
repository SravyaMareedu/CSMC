#include <stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>

int num_threads_exited = 0;
int num_of_chairs;
int num_of_students;
int num_of_tutors;
int num_of_helps;
int totreqs = 0;
#define max_students 9999999

int num_of_helps_student[max_students][2];
int arrivedStudent[max_students];
int student_ids[max_students];
int tutor_ids[max_students];
int flag = -1;
int counter=-1;
struct node{
   int data;	
   int key;
   int tutor_id;
   struct node *next;
};
struct node *head = NULL;
//struct node *current = NULL;

void insertData(int val,int count)
{
   //printf("hi\n");
   struct node* nptr = (struct node*) malloc(sizeof(struct node));
   nptr->data = val;
   nptr->key = count;
   nptr->tutor_id = 0;
   nptr->next = NULL;
   // printf("1\n");
   if(head == NULL)
   {
     head = nptr;
   }
   else
   {
   //printf("2\n");
   struct node *temp = head;
   
   struct node *temp2;
   while(temp->key <= count && temp->next!=NULL)
   {
      //printf("3");
      temp2 = temp;
      temp = temp->next;
   }
   if(temp == head)
   {
      //printf("4\n");
      if(temp->key <= count){
         //printf("5\n");
         nptr->next = temp->next;
         temp->next = nptr;
      }
      else
      {
         //printf("6\n");
         nptr->next = temp;
	 head = nptr;
      }
   }
   else if(temp->key > count)
   { 
      //printf("7\n");
      temp2->next = nptr;
      nptr->next = temp;
   }
   else
   {   
     nptr->next = temp->next;
     temp->next = nptr;
   
   }
   }
   
   /*  
   while(temp->next!=NULL){
      // printf("3");
       temp = temp->next;
   }
   temp->next = nptr;
   }
   */
   //printf("INSERTED SUCCESSFULLY\n");
}

void deleteFirstNode()
{
    //printf("deleting1\n");
    struct node *toDelete;
    if(head == NULL)
    {
       printf("Queue is empty");
    }
    else
    {
      // printf("deleting2\n");
       toDelete = head;
       head = head->next;
       //printf("Data Deleted %d\n",toDelete->data);
       free(toDelete);
    }
}


void show()
{
   struct node *temp = head;
   while(temp!=NULL)
   {
     printf("%d",temp->data);
     temp = temp->next;
   }
   printf("\n");
}

int position(int ids)
{
   struct node *temp = head;
   int count = 0;
   while(temp->data != ids)
   {
      count++;
      temp = temp->next;
   }
   return count+1;
}
int getKey(int id)
{
  struct node *temp = head;
  while(temp->data != id || temp!=NULL)
  {
     temp = temp->next;
  }
  return temp->key;
}

int firstData()
{
   struct node *temp = head;
   return temp->data;
}

int firstKey()
{
   struct node *temp = head;
   return temp->key;
}

int getTutorId(int ids)
{
  
   struct node *temp = head;
   int res;
   if(head == NULL)
     res = -1;
   while(temp->next != NULL)
   {
      if(temp->data == ids)
      {
          res = temp->tutor_id;    
	  break;
      }
      temp = temp->next;
   }
   return res;
   
 // return head->tutor_id;
}
int getFirstTutorId()
{
   return head->tutor_id;
}

void fillTutorId(int stud_id,int id)
{
   struct node *temp = head;
   while(temp!=NULL)
   {
      if(temp->data == stud_id)
      {
         temp->tutor_id = id;
	 break;
      }
      temp = temp->next;
   }
}


int siz()
{ 
  // printf("hi");
  struct node *temp = head;
  int count = 0;
  while(temp != NULL)
  {
     count++;
     temp = temp->next;
  }
  //printf("boe");
  return count;
}

int numStudtut=0;
int totalSessions = 0;
int cordinator_flag = -1;
sem_t cordinator;
sem_t tutor;
sem_t queue;

pthread_mutex_t lock;
int student_flag = -1;
int tutor_flag = -1;

void* student_func(void* p){
    //printf("student thread arrived\n");
    int id = *(int*)p;
    while(1){
    //pritnf("Student arrived \n");
    pthread_mutex_lock(&lock);
    int i;
    int pos;
    for(i=0;i<num_of_students;i++)
    {
       if(num_of_helps_student[i][0] == id) {
         pos = i;
    }
    }
    
    /*
       if(num_of_chairs > 0 || num_of_helps_student[pos][1]>=num_of_helps){
       	 num_of_helps_student[pos][1] = num_of_helps_student[pos][1] + 1;
	 printf("id %d help %d\n",id,num_of_helps_student[pos][1]);
	 }
  */	
    
    pthread_mutex_unlock(&lock);
    //printf("total helps : %d\n",num_of_helps_student[pos][1]); 
    //while(1){
    if(num_of_helps_student[pos][1] >= num_of_helps)
    {   
        // printf(" %d error : %d\n",id,num_of_helps_student[pos][1]);
         pthread_mutex_lock(&lock);
         num_threads_exited++;
	 pthread_mutex_unlock(&lock);
         
         if(num_threads_exited >= num_of_students){
	    sem_post(&cordinator); // --------- idi fill cheyali
	    //printf("posted2\n");
	 }
	 pthread_exit(NULL);
    
    }
    /*
    float sleep_time =  (float)(rand()%200)/100;
    usleep(sleep_time);
    pthread_mutex_lock(&lock);
    */
   // pthread_mutex_lock(&lock);
    
    if(num_of_chairs <= 0 )  // If the chairs are not available
    {
        //pthread_mutex_lock(&lock);
        printf("S: Student %d found no empty chair. Will try again later.\n",id);
	
    	float sleep_time = (float)(rand()%200)/100;
	usleep(sleep_time);// The student sleeps for a random amount of time  
         	
       // pthread_mutex_unlock(&lock);

	continue;
	//sem_wait(&chair);
    }
    /*
    if(num_of_chairs <= 0)
    {
       printf("S: Student %d found no empty chair. Will try again later.\n",id);
       pthread_mutex_unlock(&lock);
       continue;
    }
    
    else
    {
      pthread_mutex_unlock(&lock);
    }
    */
   
    pthread_mutex_lock(&lock);
    //while(num_of_chairs <=0 );
    if(num_of_chairs > 0)
    {
    num_of_chairs--;  	//A student occupies a chair, the number of available chairs decreases
    
    //int i;
    /*
    int j;
    int x;
    for(j=0;j<max_students;j++)
    {
      if(student_ids[j] == id)
      {
         x = j;
	 break;
      }
    }
    while(tutor_ids[x] == -1);
    */
    //while(getTutorId(id) == -1);
    int i;
    for(i=0;i<num_of_students;i++)
    {
      if(num_of_helps_student[i][0] == id)
       {
          num_of_helps_student[i][1]++;
       }
   }
    printf("S: Student %d takes a seat. Empty chairs = %d. \n",id,num_of_chairs);
    arrivedStudent[counter++] = id;
     
    //num_of_helps_student[i][1] = num_of_helps_student[i][1] + 1;
    pthread_mutex_unlock(&lock); // lock to avoid context  switching
    
    //printf("step before giving signal to cordinator and wait in chair\n");
    sem_post(&cordinator);   //Signal cordinator that student is waiting
    //printf("posted1\n");
    //sem_post(&student);
    //printf("gave signal to cordinato and waiting for tutor to signal queue\n");
    //while(getTutorId(id) == -1);
    sem_wait(&queue);
    //while(getTutorId(id)==-1);
    //printf("received sl from tutor\n");
    pthread_mutex_lock(&lock);	// Lock to avoid context switching
    //int tutorId = getTutorId(id);
   //  printf("getting tutoreds\n"); 
  
    float sleep = (float)(rand()%200)/1000;
    usleep(sleep);
    
    int data = student_ids[student_flag++];
    int tid = tutor_ids[student_flag];
    
    printf("S: Student %d received help from Tutor %d. \n", data,tid);
    //deleteFirstNode();
    //show();
   // num_of_chairs++;
   // if(num_of_chairs==1){
   // sem_post(&chair);
   // }
    pthread_mutex_unlock(&lock);  //Lock to avoid context switching
   // printf("student exited\n");
   }
   else
   {
    pthread_mutex_unlock(&lock);
}
   }
}

void *cordinator_func()
{
   //printf("cord thread started\n"); 
   while(1){
   sem_wait(&cordinator);
   //printf("hi\n");
   if(num_threads_exited == num_of_students)
   {
     // sem_wait(&cordinator);
     // printf("Hi");
      int i;
      for(i=0;i<num_of_tutors;i++)
      {
         sem_post(&tutor); // ------> idi fill cheyali
      }
      pthread_exit(NULL);
   }

  // printf("cordinator is waiting for sl from stdnt\n");  
   //printf("Cordinator received sl frm stdnt \n");
   //sem_wait(&cordinator);
   pthread_mutex_lock(&lock);

   int id = arrivedStudent[cordinator_flag++];
   
   //int i;
   int key;
   int i; 
   for(i=0;i<num_of_students;i++)
   {
      if(num_of_helps_student[i][0] == id)
      {
         
         key = num_of_helps_student[i][1] ;	 
      }
   }
   //printf("putting stdnt in queue\n");
   insertData(id,key);
   
   //printf("priority noted \n");
   int s = siz();
   totreqs++;
   printf("C: Student %d with priority %d added to the queue. Waiting students now = %d. Total requests = %d\n",id,key-1,s,totreqs);
  // show();
   sem_post(&tutor);
   pthread_mutex_unlock(&lock);
   }
   
}
int tutorsWorking = 0;

void *tutor_func(void *p)
{
   //printf("tutor arrived\n");  
   int id = *(int*)p;
   while(1){
       sem_wait(&tutor);
       if(num_threads_exited == num_of_students){
           pthread_exit(NULL);
       }
       //printf("tutor waiting for the signal from cord\n");
      
      // printf("tutor received signal from cord\n");
        
       pthread_mutex_lock(&lock);
       tutorsWorking++;
       
      // printf("tutor working increased\n");
       pthread_mutex_unlock(&lock);

      // printf("tutor exited lock\n"); 	
      // sem_post(&queue);
       float tutorSleep = (float)(rand()%200)/1000;
       usleep(tutorSleep);

       pthread_mutex_lock(&lock);
       tutorsWorking--;
       totalSessions++;
       numStudtut++;
       int stud_id = firstData();
       fillTutorId(stud_id,id);
       student_ids[flag++] = stud_id;
       tutor_ids[flag] = id;
       printf("T: Student %d tutored by Tutor %d. Students tutored now = %d. Total sessions tutored = %d.\n",stud_id,id,tutorsWorking+1, totalSessions);
      // tutorsWorking--;
       deleteFirstNode();
       num_of_chairs++;

        
       pthread_mutex_unlock(&lock);
       //show();
       sem_post(&queue);
      // printf("post avtundi\n");
       
      // printf("cross avtunfi\n");
   }
} 
int id[max_students];
int tid[max_students];
int main(int argc, const char *argv[])
 {
    num_of_students = atoi(argv[1]); // storing number of students in a variable
    num_of_tutors = atoi(argv[2]); // storing number of tutors in a variable
    num_of_chairs = atoi(argv[3]); // storing number of chairs in a variable
    num_of_helps = atoi(argv[4]); // storing max number of helps in a variable

   /*
    num_of_helps_student = malloc((num_of_students)*sizeof(int));
    arrivedStudent = malloc(num_of_students*sizeof(int));
    student_ids = malloc(num_of_students*sizeof(int));
    tutor_ids = malloc(num_of_students*sizeof(int));
    */
    /*
    printf("%d\n",num_of_students);
    printf("%d\n",num_of_tutors);
    printf("%d\n",num_of_chairs);
    printf("%d\n",num_of_helps);
    */
  
    sem_init(&queue,0,0);
    sem_init(&cordinator,0,0);
    sem_init(&tutor,0,0);
    //sem_init(&student,0,0);
    //sem_init(&chair,0,0);
    pthread_mutex_init(&lock,NULL);
         
      
    pthread_t cord; // allocating cordinator thread
    pthread_t students[num_of_students]; //allocating student thread ids
    pthread_t tutors[num_of_tutors]; // allocating tutor thread ids
    // creating cordinator thread
    assert(pthread_create(&cord,NULL,cordinator_func, NULL)==0);
    // printf("hi");
    int i;
    //creating student threads
    for(i=0;i<num_of_students;i++)
    {
      student_ids[i] = i+1;
      id[i] = i+1;
      num_of_helps_student[i][0] = i+1;
      num_of_helps_student[i][1] = 0;
      assert(pthread_create(&students[i], NULL,student_func, &id[i])==0);  
    }
    //creating tutor threads
    for(i=0;i<num_of_tutors;i++)
    {
      tutor_ids[i] = i+1;
      tid[i] = i+1;
      assert(pthread_create(&tutors[i],NULL,tutor_func,&tid[i])==0);
    }
    
    // printf("From the main function, the thread id = %d ", cord);
    // Join threads
    assert(pthread_join(cord,NULL)==0);

    for(i=0;i<num_of_students;i++)
    {
       assert(pthread_join(students[i],NULL)==0);
    }
    for(i=0;i<num_of_tutors;i++)
    {
       assert(pthread_join(tutors[i],NULL)==0);
    }
    return 0;
}
