#include "os2021_thread_api.h"

struct itimerval Signaltimer;
struct queue *high_ready_queue = NULL, *medium_ready_queue = NULL, *low_ready_queue = NULL,
                  *event_high_queue = NULL, *event_medium_queue = NULL, *event_low_queue = NULL,
                   *running_queue = NULL, *terminate_queue = NULL, *waiting_queue = NULL,
                    *current_thread, *previous_thread = NULL, *terminate_next_thread = NULL;

ucontext_t dispatch_context;
ucontext_t timer_context;



int Time_Quantum = 0, current_time = 0, waiting_time = 0, total_time = 0, difference = 0;;
int TID = 0;




void AddReadyQueue(struct T_info *new_thread)
{
    if((strcmp(new_thread->priority, "H")) == 0)
    {

        //insert  first high queue element
        if(high_ready_queue->thread_info == NULL)
        {
            //high_ready_queue = malloc(sizeof(struct queue));
            high_ready_queue->thread_info = new_thread;
            high_ready_queue->next = malloc(sizeof(struct queue));
            high_ready_queue->next->thread_info = NULL;
            high_ready_queue->next->next = NULL;



        }
        //insert other high queue element
        else
        {
            current_thread = high_ready_queue;
            while(current_thread->thread_info != NULL)
                current_thread = current_thread->next;
            current_thread->thread_info = new_thread;
            current_thread->next = malloc(sizeof(struct queue));
            current_thread->next->thread_info = NULL;
        }
    }

    if((strcmp(new_thread->priority, "M")) == 0)
    {
        //insert first queue element
        if(medium_ready_queue->thread_info == NULL)
        {
            //medium_ready_queue = malloc(sizeof(struct queue));
            medium_ready_queue->thread_info = new_thread;
            medium_ready_queue->next = malloc(sizeof(struct queue));
            medium_ready_queue->next->thread_info = NULL;
            medium_ready_queue->next->next = NULL;

        }
        //insert other queue element
        else
        {
            current_thread = medium_ready_queue;
            while(current_thread->thread_info != NULL)
                current_thread = current_thread->next;
            current_thread->thread_info = new_thread;
            current_thread->next = malloc(sizeof(struct queue));
            current_thread->next->thread_info = NULL;
        }
    }

    if((strcmp(new_thread->priority, "L")) == 0)
    {
        //insert  first high queue element
        if(low_ready_queue->thread_info == NULL)
        {
            //low_ready_queue = malloc(sizeof(struct queue));
            low_ready_queue->thread_info = new_thread;
            low_ready_queue->next = malloc(sizeof(struct queue));
            low_ready_queue->next->thread_info = NULL;
            low_ready_queue->next->next = NULL;


        }
        //insert other high queue element
        else
        {
            current_thread = low_ready_queue;
            while(current_thread->thread_info != NULL)
                current_thread = current_thread->next;
            current_thread->thread_info = new_thread;
            current_thread->next = malloc(sizeof(struct queue));
            current_thread->next->thread_info = NULL;
            current_thread->next->next = NULL;
        }
    }



}
void AddTerminateQueue(struct T_info *new_thread)
{

    current_thread = terminate_queue->next;
    while(current_thread->thread_info != NULL)
        current_thread = current_thread->next;
    current_thread->thread_info = new_thread;

    //create new space for terminate queue element
    current_thread->next = malloc(sizeof(struct queue));
    current_thread->next->thread_info = NULL;
    current_thread->next->next = NULL;


}


int OS2021_ThreadCreate(char *job_name, char *p_function, char *priority, int cancel_mode)
{

    TID = TID + 1;
    struct T_info *new_thread = malloc(sizeof(struct T_info));
    new_thread->job_name = malloc(strlen(job_name)+1);
    new_thread->p_function = malloc(strlen(p_function)+1);
    new_thread->priority = malloc(strlen(priority)+1);
    new_thread->B_Priority = malloc(strlen(priority)+1);
    new_thread->state = "READY";
    //use strcpy and don't reference to an address

    strcpy(new_thread->job_name, job_name);
    strcpy(new_thread->p_function, p_function);
    strcpy(new_thread->priority, priority);
    strcpy(new_thread->B_Priority, priority);

    new_thread->cancel_mode = cancel_mode;
    new_thread->TID = TID;
    new_thread->testcancel = false;
    new_thread->queueing_time = 0;
    new_thread->event_waiting__time = 0;
    new_thread->waiting_queue_target_time = 0;
    new_thread->waiting_queue_enter_time = 0;
    if(TID != 1)
        new_thread->queueing_time += 10;



    if((strcmp(priority, "H")) == 0)
        new_thread->time_quantum = 100;
    else if((strcmp(priority, "M")) == 0)
        new_thread->time_quantum = 200;
    else if((strcmp(priority, "L")) == 0)
        new_thread->time_quantum = 300;


    AddReadyQueue(new_thread);






    if((strcmp(p_function, "Function1")) == 0)
        CreateContext(&(new_thread->thread), NULL, Function1);
    else if((strcmp(p_function, "Function2")) == 0)
        CreateContext(&(new_thread->thread), NULL, Function2);
    else if((strcmp(p_function, "Function3")) == 0)
        CreateContext(&(new_thread->thread), NULL, Function3);
    else if((strcmp(p_function, "Function4")) == 0)
        CreateContext(&(new_thread->thread), NULL, Function4);
    else if((strcmp(p_function, "Function5")) == 0)
        CreateContext(&(new_thread->thread), NULL, Function5);
    else if((strcmp(p_function, "ResourceReclaim")) == 0)
        CreateContext(&(new_thread->thread), NULL, ResourceReclaim);
    else
        return -1;


    //setcontext(&(new_thread->thread));
    return TID;


}

//someone wants to cancel other one,
void OS2021_ThreadCancel(char *job_name)
{

    //select removed thread from high ready queue

    if(high_ready_queue->thread_info != NULL)
    {
        current_thread = high_ready_queue;
        while((strcmp(current_thread->thread_info->job_name, job_name)) != 0)
        {
            if(current_thread->next == NULL)
                break;
            previous_thread = current_thread;
            current_thread = current_thread->next;
        }

        //previous_thread->next = current_thread->next;

    }
    //select removed thread from medium ready queue
    if(medium_ready_queue->thread_info != NULL)
    {
        current_thread = medium_ready_queue;
        while((strcmp(current_thread->thread_info->job_name, job_name)) != 0)
        {
            if(current_thread->next == NULL)
                break;
            previous_thread = current_thread;
            current_thread = current_thread->next;
        }
        //previous_thread->next = current_thread->next;


    }
    //select removed thread from low ready queue
    if(low_ready_queue->thread_info != NULL)
    {
        current_thread = low_ready_queue;
        while((strcmp(current_thread->thread_info->job_name, job_name)) != 0)
        {
            if(current_thread->next == NULL)
                break;
            previous_thread = current_thread;
            current_thread = current_thread->next;

        }


        //previous_thread->next = malloc(sizeof(struct queue));
        //previous_thread->next = current_thread->next;

    }

    if(current_thread->thread_info->cancel_mode == 0)
    {
        running_queue->thread_info->state = "TERMINATE";
        AddTerminateQueue(current_thread->thread_info);
        previous_thread->next = current_thread->next;

    }
    else if(current_thread->thread_info->cancel_mode == 1)
        current_thread->thread_info->testcancel = true;

}

void OS2021_ThreadWaitEvent(int event_id)
{
    //Add priority and event_id queue



    struct T_info *running_thread_info = running_queue->thread_info;
    printf("%s wants to wait for event %d\n", running_thread_info->job_name, event_id);

    if((strcmp(running_thread_info->priority, "H")) == 0)
    {
        current_thread = event_high_queue;
        while(current_thread->thread_info != NULL)
        {
            current_thread = current_thread->next;
        }

        current_thread->thread_info = running_thread_info;
        current_thread->thread_info->state = "WAITING";
        current_thread->thread_info->event_id = event_id;

        current_thread->next = malloc(sizeof(struct queue));;
        current_thread->next->thread_info = NULL;
        current_thread->next->next = NULL;
    }

    if((strcmp(running_thread_info->priority, "M")) == 0)
    {

        current_thread = event_medium_queue;
        //find the final element of event queue
        while(current_thread->thread_info != NULL)
        {
            current_thread = current_thread->next;
        }

        current_thread->thread_info = running_thread_info;
        current_thread->thread_info->state = "WAITING";
        current_thread->thread_info->event_id = event_id;

        current_thread->next = malloc(sizeof(struct queue));;
        current_thread->next->thread_info = NULL;
        current_thread->next->next = NULL;

    }
    if((strcmp(running_thread_info->priority, "L")) == 0)
    {

        current_thread = event_low_queue;
        //find the final element of event queue
        while(current_thread->thread_info != NULL)
        {
            current_thread = current_thread->next;
        }

        current_thread->thread_info = running_thread_info;
        current_thread->thread_info->state = "WAITING";
        current_thread->thread_info->event_id = event_id;

        current_thread->next = malloc(sizeof(struct queue));;
        current_thread->next->thread_info = NULL;
        current_thread->next->next = NULL;

    }

    if(Time_Quantum < running_queue->thread_info->time_quantum)
    {


        if((strcmp(running_queue->thread_info->priority, "M")) == 0)
        {

            printf("\nThe priority of thread %s is changed from M to H\n", running_queue->thread_info->job_name);
            running_queue->thread_info->priority = "H";
            running_queue->thread_info->time_quantum = 100;

        }

        else if((strcmp(running_queue->thread_info->priority, "L")) == 0)
        {
            printf("\nThe priority of thread %s is changed from L to M", running_queue->thread_info->job_name);
            running_queue->thread_info->priority = "M";
            running_queue->thread_info->time_quantum = 200;

        }
    }





    swapcontext(&(running_queue->thread_info->thread), &dispatch_context);





    //if((strcmp(running_thread_info->priority, "L")) == 0)

}

void OS2021_ThreadSetEvent(int event_id)
{
    struct T_info *running_thread_info = running_queue->thread_info;


    //check these three while loop
    if((event_high_queue->thread_info) != NULL)
    {

        current_thread = event_high_queue;
        previous_thread = event_high_queue;
        //while loop will break, if find event_id target or find NULL of end of event_queue
        while(current_thread->thread_info->event_id != event_id)
        {
            previous_thread = current_thread;
            current_thread = current_thread->next;
            if(current_thread->thread_info == NULL)
                break;
        }

        if(current_thread->thread_info != NULL)
        {
            printf("\n%s changes the status of %s to READY \n", running_thread_info->job_name, current_thread->thread_info->job_name);
            //check this line
            if(event_high_queue == current_thread)
                event_high_queue = current_thread->next;
            else
                previous_thread->next = current_thread->next;
            AddReadyQueue(current_thread->thread_info);
            return;
        }




    }

    if((event_medium_queue->thread_info) != NULL)
    {

        current_thread = event_medium_queue;
        previous_thread = event_medium_queue;
        //while loop will break, if find event_id target or find NULL of end of event_queue
        while(current_thread->thread_info->event_id != event_id)
        {
            previous_thread = current_thread;
            current_thread = current_thread->next;
            if(current_thread->thread_info == NULL)
                break;
        }

        if(current_thread->thread_info != NULL)
        {
            printf("\n%s changes the status of %s to READY \n", running_thread_info->job_name, current_thread->thread_info->job_name);
            if(event_medium_queue == current_thread)
                event_medium_queue = current_thread->next;
            else
                previous_thread->next = current_thread->next;
            AddReadyQueue(current_thread->thread_info);
            return;
        }




    }

    if((event_low_queue->thread_info) != NULL)
    {

        current_thread = event_low_queue;
        previous_thread = event_low_queue;
        //while loop will break, if find event_id target or find NULL of end of event_queue
        while(current_thread->thread_info->event_id != event_id)
        {
            previous_thread = current_thread;
            current_thread = current_thread->next;
            if(current_thread->thread_info == NULL)
                break;
        }

        if(current_thread->thread_info != NULL)
        {
            printf("\n%s changes the status of %s to READY \n", running_thread_info->job_name, current_thread->thread_info->job_name);
            if(event_low_queue == current_thread)
                event_low_queue = current_thread->next;
            else
                previous_thread->next = current_thread->next;
            AddReadyQueue(current_thread->thread_info);
            return;
        }
    }

    /*if((event_medium_queue->thread_info) != NULL){ this lien

        current_thread = event_medium_queue;
        current_thread->thread_info->state = "READY";

        printf("\n%s changes the status of %s to READY \n", running_thread_info->job_name, current_thread->thread_info->job_name);
        //Add the element to ready queue
        AddReadyQueue(current_thread->thread_info);

        //move the second element to the first position
        current_thread = current_thread->next;

        event_medium_queue->thread_info = current_thread->thread_info;
        event_medium_queue->next = current_thread->next;

     }*///this lien

    //if((strcmp(running_thread_info->priority, "H")) == 0)
    /*if((strcmp(running_thread_info->priority, "M")) == 0){

       current_thread = event_medium_queue;
       //find the final element of event queue
       while(current_thread->thread_info != NULL){
           current_thread = current_thread->next;
       }
       current_thread->thread_info = running_thread_info;
       current_thread->next = NULL;

       running_queue->thread_info = NULL;

    }*/
    //if((strcmp(running_thread_info->priority, "L")) == 0)
}

void OS2021_ThreadWaitTime(int msec)
{



    //waiting_time = msec * 10;
    //current_time = total_time;

    //total waiting time
    running_queue->thread_info->waiting_queue_target_time = msec * 10;
    //the time enter waiting queue
    running_queue->thread_info->waiting_queue_enter_time = total_time;
    running_queue->thread_info->state = "WAITING";

    if((strcmp(running_queue->thread_info->priority, "L")) == 0)
    {
        printf("\nThe priority of thread %s is changed from L to M\n", running_queue->thread_info->job_name);
        running_queue->thread_info->priority = "M";
        running_queue->thread_info->time_quantum = 200;
    }
    else if((strcmp(running_queue->thread_info->priority, "M")) == 0)
    {
        printf("\nThe priority of thread %s is changed from M to H\n", running_queue->thread_info->job_name);
        running_queue->thread_info->priority = "H";
        running_queue->thread_info->time_quantum = 100;
    }



    //waiting_queue->thread_info = running_queue->thread_info;
    current_thread = waiting_queue;
    while(current_thread->thread_info != NULL)
    {
        current_thread = current_thread->next;
    }
    current_thread->thread_info = running_queue->thread_info;
    current_thread->next = malloc(sizeof(struct queue));
    current_thread->next->thread_info = NULL;
    current_thread->next->next = NULL;



    Time_Quantum = 0;

    swapcontext(&(running_queue->thread_info->thread), &dispatch_context);



}

void OS2021_DeallocateThreadResource()
{
    if(terminate_queue->next->thread_info != NULL)
    {
        while(terminate_queue->next->thread_info != NULL)
        {
            terminate_next_thread = terminate_queue->next->next;
            printf("\nThe memory space by %s has been released.\n", terminate_queue->next->thread_info->job_name);
            free(terminate_queue->next);
            terminate_queue->next = terminate_next_thread;
        }

    }
}

void OS2021_TestCancel()
{
    if(running_queue->thread_info->testcancel == true)
    {
        running_queue->thread_info->state = "TERMINATE";
        AddTerminateQueue(running_queue->thread_info);
        swapcontext(&running_queue->thread_info->thread, &dispatch_context);
    }

}

void CreateContext(ucontext_t *context, ucontext_t *next_context, void *func)
{

    getcontext(context);
    context->uc_stack.ss_sp = malloc(STACK_SIZE);
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_flags = 0;
    context->uc_link = next_context;
    makecontext(context,(void (*)(void))func,0);


}





void ParseJson()
{

    FILE *fp;
    char buffer[1024];

    char *job_name, *p_function, *priority, *cancel_mode_name;
    int cancel_mode, create_context = 0;

    //json_object *parsed_json;
    //json_object *Threads;

    fp = fopen("init_threads.json","r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    char *ptr = strstr(buffer, "\"name\" : ");


    char *token = strtok(ptr, ",\n			");



    while (token != NULL)
    {
        if(strlen(token) != 1)
        {
            if(token[1] == 'n')
            {



                job_name = strstr(token, " \"");
                job_name = strstr(job_name, "\"");
                sscanf(job_name, "\"%s\"", job_name);
                job_name[strlen(job_name) - 1] = NULL;


            }
            else if(token[1] == 'e')
            {
                p_function = strstr(token, " \"");
                p_function = strstr(p_function, "\"");
                sscanf(p_function, "\"%s\"", p_function);
                p_function[strlen(p_function) - 1] = NULL;


            }
            else if(token[1] == 'p')
            {
                priority = strstr(token, " \"");
                priority = strstr(priority, "\"");
                sscanf(priority, "\"%s\"", priority);
                priority[strlen(priority) - 1] = NULL;


            }
            else if(token[1] == 'c')
            {
                cancel_mode_name = strstr(token, " \"");
                cancel_mode_name = strstr(cancel_mode_name, "\"");
                sscanf(cancel_mode_name, "\"%s\"", cancel_mode_name);
                cancel_mode_name[strlen(cancel_mode_name) - 1] = NULL;

                cancel_mode = atoi(cancel_mode_name);

            }



            create_context++;
            if(create_context == 4)
            {
                OS2021_ThreadCreate(job_name, p_function, priority, cancel_mode);
                create_context = 0;
            }

        }
        token = strtok(NULL, ",\n			");
    }


    //Add reclaimer thread
    OS2021_ThreadCreate("reclaimer", "ResourceReclaim", "L", 1);




}

void ShowQueue(int sig_num)
{
    struct T_info *thread = NULL;
    //ready queue content

    printf("\n========================\n");
    printf("High ready queue: ");
    if(high_ready_queue->thread_info != NULL)
    {

        for(current_thread = high_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }
    }


    printf("\n\nMedium ready queue: ");
    if(medium_ready_queue->thread_info != NULL)
    {

        for(current_thread = medium_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }
    }

    printf("\n\nLow ready queue: ");
    if(low_ready_queue->thread_info != NULL)
    {

        for(current_thread = low_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }
    }

    printf("\n\nrunning_queue queue: ");
    if(running_queue->thread_info != NULL)
    {



        thread = running_queue->thread_info;
        printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
               thread->priority, thread->state, thread->cancel_mode,
               thread->TID, thread->time_quantum);


    }

    printf("\n\nevent_high_queue: ");
    if(event_high_queue->thread_info != NULL)
    {
        for(current_thread = event_high_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }





    }

    printf("\n\nevent_medium_queue: ");
    if(event_medium_queue->thread_info != NULL)
    {
        for(current_thread = event_medium_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }





    }

    printf("\n\nevent_low_queue: ");
    if(event_low_queue->thread_info != NULL)
    {
        for(current_thread = event_low_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }





    }

    printf("\n\nwaiting_queue: ");
    if(waiting_queue->thread_info != NULL)
    {



        thread = waiting_queue->thread_info;
        printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
               thread->priority, thread->state, thread->cancel_mode,
               thread->TID, thread->time_quantum);


    }

    printf("\n\nterminate_queue: ");
    if(terminate_queue->next->thread_info != NULL)
    {
        for(current_thread = terminate_queue->next; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            thread = current_thread->thread_info;
            printf("%s %s %s %s %d %d %d | ", thread->job_name, thread->p_function,
                   thread->priority, thread->state, thread->cancel_mode,
                   thread->TID, thread->time_quantum);
        }

    }
    printf("\n\nTQ :%d\nTotal_time: %d\n\n========================\n", Time_Quantum, total_time);

}

void ShowThreadMessage()
{
    printf("\n******************************************************************************************\n");
    printf("\n*     TID   Name        state       B_Priority      C_Priority      Q_Time      W_Time    *\n");

    if(running_queue->thread_info != NULL)
    {
        printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
               running_queue->thread_info->TID,
               running_queue->thread_info->job_name,
               running_queue->thread_info->state,
               running_queue->thread_info->B_Priority,
               running_queue->thread_info->priority,
               running_queue->thread_info->queueing_time,
               running_queue->thread_info->event_waiting__time
              );
    }

    if(high_ready_queue->thread_info != NULL)
    {

        for(current_thread = high_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }



    if(medium_ready_queue->thread_info != NULL)
    {

        for(current_thread = medium_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }


    if(low_ready_queue->thread_info != NULL)
    {

        for(current_thread = low_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }


    if(event_high_queue->thread_info != NULL)
    {
        for(current_thread = event_high_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {
            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }


    if(event_medium_queue->thread_info != NULL)
    {
        for(current_thread = event_medium_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }

    if(event_low_queue->thread_info != NULL)
    {
        for(current_thread = event_low_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }

    if(waiting_queue->thread_info != NULL)
    {
        for(current_thread = waiting_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
        {

            printf("*     %-5d %-11s %-11s %-15s %-15s %-11d %-10d*\n",
                   current_thread->thread_info->TID,
                   current_thread->thread_info->job_name,
                   current_thread->thread_info->state,
                   current_thread->thread_info->B_Priority,
                   current_thread->thread_info->priority,
                   current_thread->thread_info->queueing_time,
                   current_thread->thread_info->event_waiting__time
                  );
        }
    }

    printf("\n******************************************************************************************\n");

}

void SetTimer()
{
    Signaltimer.it_interval.tv_sec = 0;
    Signaltimer.it_interval.tv_usec = 10000;

    Signaltimer.it_value.tv_sec = 0;
    Signaltimer.it_value.tv_usec = 10000;
    if(setitimer(ITIMER_REAL, &Signaltimer, NULL) < 0)
    {
        printf("ERROR SETTING TIME SIGALRM!\n");
    }


}



void ReadyToRunning()
{


    struct T_info *selected_thread = NULL;

    //make sure running queue is empty and ready queue is not empty

    if((running_queue->thread_info) == NULL && (high_ready_queue->thread_info) != NULL)
    {

        current_thread = high_ready_queue;
        //select first element in ready queue
        selected_thread = current_thread->thread_info;
        //remove first element in ready queue
        current_thread = current_thread->next;
        high_ready_queue->thread_info = current_thread->thread_info;
        high_ready_queue->next = current_thread->next;
        //change first element's state to RUNNING
        selected_thread->state = "RUNNING";




        //Add selected element to runnging queue
        running_queue->thread_info = selected_thread;





    }
    //select from medium queue
    if((running_queue->thread_info) == NULL && (medium_ready_queue->thread_info) != NULL)
    {

        current_thread = medium_ready_queue;
        //select first element in ready queue
        selected_thread = current_thread->thread_info;
        //remove first element in ready queue
        current_thread = current_thread->next;
        medium_ready_queue->thread_info = current_thread->thread_info;
        medium_ready_queue->next = current_thread->next;
        //change first element's state to RUNNING
        selected_thread->state = "RUNNING";



        //Add selected element to runnging queue
        running_queue->thread_info = selected_thread;





    }

    //select from low queue
    if((running_queue->thread_info) == NULL && (low_ready_queue->thread_info) != NULL)
    {

        current_thread = low_ready_queue;
        //select first element in ready queue
        selected_thread = current_thread->thread_info;
        //remove first element in ready queue
        current_thread = current_thread->next;
        low_ready_queue->thread_info = current_thread->thread_info;
        low_ready_queue->next = current_thread->next;
        //change first element's state to RUNNING
        selected_thread->state = "RUNNING";
        //Add selected element to runnging queue



        running_queue->thread_info = selected_thread;





    }

    swapcontext(&(dispatch_context), &(running_queue->thread_info->thread));


}
void check()
{

    current_thread = waiting_queue;
    previous_thread = waiting_queue;
    //check this while loop
    while(current_thread->thread_info != NULL)
    {
        difference = (total_time - current_thread->thread_info->waiting_queue_enter_time);
        if(difference > current_thread->thread_info->waiting_queue_target_time)
        {
            AddReadyQueue(current_thread->thread_info);
            if(waiting_queue == current_thread)
                waiting_queue = current_thread->next;
            else
                previous_thread->next = current_thread->next;

        }
        previous_thread = current_thread;
        current_thread = current_thread->next;
    }


    if((running_queue->thread_info) != NULL && (Time_Quantum >= running_queue->thread_info->time_quantum))
    {

        //thread give up cpu, change priority, ex:from M to H
        if((strcmp(running_queue->thread_info->priority, "H")) == 0)
        {

            running_queue->thread_info->priority = "M";
            running_queue->thread_info->time_quantum = 200;
            printf("\nThe priority of thread %s is changed from H to M\n", running_queue->thread_info->job_name);
        }

        else if((strcmp(running_queue->thread_info->priority, "M")) == 0)
        {
            running_queue->thread_info->priority = "L";
            running_queue->thread_info->time_quantum = 300;
            printf("\nThe priority of thread %s is changed from M to L\n\n", running_queue->thread_info->job_name);
        }



        running_queue->thread_info->state = "READY";
        AddReadyQueue(running_queue->thread_info);


        Time_Quantum = 0;

        swapcontext(&running_queue->thread_info->thread, &dispatch_context);

        //return;//check this line
    }
    //release waiting queue element



    /*if((total_time - current_time) > waiting_time && waiting_queue->thread_info != NULL){

        AddReadyQueue(waiting_queue->thread_info);//segmentation
        waiting_queue->thread_info = NULL;
    }*/

}



void scheduler(int sig_num)
{



    while(1)
    {
        ReadyToRunning();
        running_queue->thread_info = NULL;
    }




}

void timer()
{
    Time_Quantum += 10;
    total_time += 10;


    if(high_ready_queue->thread_info != NULL)
    {

        for(current_thread = high_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->queueing_time += 10;
    }



    if(medium_ready_queue->thread_info != NULL)
    {

        for(current_thread = medium_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->queueing_time += 10;
    }


    if(low_ready_queue->thread_info != NULL)
    {

        for(current_thread = low_ready_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->queueing_time += 10;
    }


    if(event_high_queue->thread_info != NULL)
    {
        for(current_thread = event_high_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->event_waiting__time += 10;
    }


    if(event_medium_queue->thread_info != NULL)
    {
        for(current_thread = event_medium_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->event_waiting__time += 10;
    }

    if(event_low_queue->thread_info != NULL)
    {
        for(current_thread = event_low_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->event_waiting__time += 10;
    }

    if(waiting_queue->thread_info != NULL)
    {
        for(current_thread = waiting_queue; current_thread->thread_info != NULL; current_thread = current_thread->next)
            current_thread->thread_info->event_waiting__time += 10;
    }

    check();

}

void signal_process()
{
    //ctrl + \, ShowQueue, eventqueue
    signal(SIGQUIT, ShowQueue);
    //ctrl + z, show thread state
    signal(SIGTSTP, ShowThreadMessage);
    signal(SIGALRM, timer);

}


void StartSchedulingSimulation()
{



    CreateContext(&dispatch_context, NULL, &scheduler);
    //setcontext(&dispatch_context);
    high_ready_queue = malloc(sizeof(struct queue));
    high_ready_queue->thread_info = NULL;
    high_ready_queue->next = NULL;
    medium_ready_queue = malloc(sizeof(struct queue));
    medium_ready_queue->thread_info = NULL;
    medium_ready_queue->next = NULL;
    low_ready_queue = malloc(sizeof(struct queue));
    low_ready_queue->thread_info = NULL;
    low_ready_queue->next = NULL;

    running_queue = malloc(sizeof(struct queue));
    running_queue->thread_info = NULL;
    running_queue->next = NULL;

    event_high_queue = malloc(sizeof(struct queue));
    event_high_queue->thread_info = NULL;
    event_high_queue->next = NULL;

    event_medium_queue = malloc(sizeof(struct queue));
    event_medium_queue->thread_info = NULL;
    event_medium_queue->next = NULL;

    event_low_queue = malloc(sizeof(struct queue));
    event_low_queue->thread_info = NULL;
    event_low_queue->next = NULL;

    terminate_queue = malloc(sizeof(struct queue));
    terminate_queue->thread_info = malloc(sizeof(struct T_info));
    terminate_queue->next = malloc(sizeof(struct queue));
    terminate_queue->next->thread_info = NULL;
    terminate_queue->next->next = NULL;


    waiting_queue = malloc(sizeof(struct queue));
    waiting_queue->thread_info = NULL;
    waiting_queue->next = NULL;

    /*previous_thread = malloc(sizeof(struct queue));
    previous_thread->thread_info = NULL;
    previous_thread->next = NULL;

    current_thread = malloc(sizeof(struct queue));
    current_thread->thread_info = NULL;
    current_thread->next = NULL;

    terminate_next_thread = malloc(sizeof(struct queue));
    terminate_next_thread->thread_info = NULL;
    terminate_next_thread->next = NULL;*/
    ParseJson();
    signal_process();
    SetTimer();


    setcontext(&dispatch_context);

    while(1);



}
