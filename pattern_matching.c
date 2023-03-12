#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pattern_matching.h"

int pm_init(pm_t * fsmTree){// Initializes the fsm parameters (the fsm itself should be allocated). init zero state
    if(fsmTree==NULL){return -1 ;}// if the fsm is not allocated return -1
    fsmTree->newstate=1;// set the id of the next state to 1
    fsmTree->zerostate=(pm_state_t*)malloc(sizeof (pm_state_t));// allocate the zerostate
    if(fsmTree->zerostate==NULL){return -1 ;}// if the zero state is still null return -1
    fsmTree->zerostate->output=(dbllist_t*)malloc(sizeof(dbllist_t));// allocate the zerostate out put
    fsmTree->zerostate->_transitions=(dbllist_t*)malloc(sizeof(dbllist_t));// allocate the transition of the zero state
    fsmTree->zerostate->id=0;//set id to 0
    fsmTree->zerostate->depth=0;// set depth to 0
    fsmTree->zerostate->fail=NULL;// set the fail state to null
    dbllist_init(fsmTree->zerostate->output);// initintlize the list of the zero state
    dbllist_init(fsmTree->zerostate->_transitions);// intintlize the transtion
    return 0;
}
int pm_addstring(pm_t *fsmTree, unsigned char *str, _size_t n)
{
    int i;
    if ((fsmTree == NULL) || (str == NULL))
        return -1;
    pm_state_t *current = fsmTree->zerostate;
    pm_state_t *temp;

    for (i = 0; i < n; i++)
    {
        temp = pm_goto_get(current, str[i]);
        if (temp == NULL)
        {
            temp = (pm_state_t *)malloc(sizeof(pm_state_t));
            if (temp == NULL)
                return -1;
            temp->id = fsmTree->newstate;
            fsmTree->newstate += 1;
            temp->depth = i + 1;
            temp->output = (dbllist_t *)malloc(sizeof(dbllist_t));
            if (temp->output == NULL)
                return -1;
            dbllist_init(temp->output);
            temp->fail = NULL;
            temp->_transitions = (dbllist_t *)malloc(sizeof(dbllist_t));
            if (temp->_transitions == NULL)
                return -1;
            dbllist_init(temp->_transitions);
            if (pm_goto_set(current, str[i], temp) == -1)
                return -1;
        }
        current = temp;
    }

    // enter output
    if (dbllist_append(current->output, str) == -1)
        return -1;
    return 0;
}

int pm_makeFSM(pm_t *fsmTree){//Finalizes construction by setting up the failrue transitions, as well as the goto transitions of the zerostate.
    dbllist_t *Queue=(dbllist_t*) malloc(sizeof(dbllist_t));// allocate queue
    dbllist_t  *fList=fsmTree->zerostate->_transitions;// make a failler list as the zero state to the transtion
    dbllist_node_t *temp=dbllist_head(fList);// get the head of the list and set is as a temp node
    if(Queue==NULL){// if the queue is null return -1 and return -1
        pm_destroy(fsmTree);
        return -1;
    }
    dbllist_init(Queue);// intitlize the queue
    for (int i = 0; i < dbllist_size(fList); ++i) {// set the fail of the first level as the zero state
        pm_state_t* tempState=((pm_labeled_edge_t*)dbllist_data(temp))->state;// get the data from the temp as the temp state
        if(dbllist_append(Queue,tempState)==-1){// append the tempstate to the queue and if it fails destroy tree and return -1
            pm_destroy(fsmTree);
            return -1;
        }
        temp=dbllist_next(temp);//set next to temp as the temp
        tempState->fail=fsmTree->zerostate;// set the fail as the zero state
    }
    while(dbllist_head(Queue)!=NULL)//other level
    {

        pm_state_t *state ;

        if(Queue==NULL)
            return -1;
        if(dbllist_head(Queue)==NULL)//nothing to pop
            return-1;
        else
        {

            dbllist_node_t *tempHead=dbllist_head(Queue);
            void *data =dbllist_data(tempHead);
            dbllist_node_t *next=dbllist_next(dbllist_head(Queue));
            free(tempHead);
            dbllist_head(Queue)=next;
            dbllist_size(Queue)=dbllist_size(Queue)-1;
            state=(pm_state_t*)data;//take the first state in queue
        }
        if(state==NULL)
            return -1;//pop was unsuccessful
        temp=dbllist_head(state->_transitions);
        int i=0;
        while(temp!=NULL)
        {
            pm_labeled_edge_t* edge=(pm_labeled_edge_t*)dbllist_data(temp);
            pm_state_t* fail_state=state->fail;//fail state-go to "father"
            pm_state_t* goTo_state= pm_goto_get(fail_state,edge->label);//next state after fail state
            while(goTo_state==NULL)
            {
                if(fail_state->depth==0)//if i in the root
                {
                    goTo_state=fsmTree->zerostate;
                    break;
                }
                fail_state=fail_state->fail;
                goTo_state= pm_goto_get(fail_state,edge->label);
            }
            edge->state->fail=goTo_state;//connect my state to fail state
            i=0;
            int size=dbllist_size(goTo_state->output);
            dbllist_node_t *ptr=dbllist_head(goTo_state->output);
            for(i=0;i<size;i++)
            {
                if(dbllist_append(edge->state->output,dbllist_data(ptr))==-1)//if addition was unsuccessful
                    return -1;
                ptr=dbllist_next(ptr);
            }
           dbllist_append(Queue ,edge->state);//add state "son" to the queue
           if(edge->state->fail->id!=0)
            printf("Setting f(%u) = %u\n",edge->state->id,edge->state->fail->id);
            temp=dbllist_next(temp);
        }

    }
    free(Queue);
    return 0;
}




int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){//Set a transition arrow from this from_state, via a symbol, to a to_state. will be used in the pm_addstring and pm_makeFSM functions.
    if(from_state==NULL||to_state==NULL)
        return -1;
    pm_labeled_edge_t* Edge=(pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
    if(Edge==NULL)
        return -1;//create node was unsuccessful
    Edge->label=symbol;
    Edge->state=to_state;
    if(dbllist_append(from_state->_transitions,Edge)==-1)//add connection between 2 state
        return -1 ;
    else
    {
        printf("Allocating state %u \n",Edge->state->id);
        printf("%u -> %c-> %u\n",from_state->id,Edge->label,to_state->id);
        return 0;
    }
}
pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){//Returns the transition state.
    if(state==NULL)
        return NULL;
    dbllist_node_t *temp=dbllist_head(state->_transitions);
    pm_labeled_edge_t* edge;
    while(temp!=NULL)//passing all the son
    {
        edge=(pm_labeled_edge_t*)dbllist_data(temp);
        if(edge->label==symbol)//if the son exist
        {
            return edge->state;
        }
        temp=dbllist_next(temp);
    }

    return NULL;
}
dbllist_t* pm_fsm_search(pm_state_t *state,unsigned char *str,_size_t n){
    if (state == NULL || str == NULL) {
        return NULL;
    }

    dbllist_t* searchList = (dbllist_t*) malloc(sizeof(dbllist_t));
    if (searchList == NULL) {
        return NULL;
    }
    dbllist_init(searchList);
    for (int i = 0; i < n; i++) {
        pm_state_t* tState = pm_goto_get(state, str[i]);
        pm_state_t* fState = state;

        while (tState == NULL) {
            if (fState->depth == 0) {
                tState = fState;
                break;
            }
            fState = fState->fail;
            if (fState == NULL) {
                dbllist_destroy(searchList, DBLLIST_FREE_DATA);
                return NULL;
            }
            tState = pm_goto_get(fState, str[i]);
        }

        if (dbllist_head(tState->output) != NULL) {
            dbllist_node_t* temp = dbllist_head(tState->output);
            for (int j = 0; j < dbllist_size(tState->output); j++) {
                pm_match_t* find = (pm_match_t*) malloc(sizeof(pm_match_t));
                if (find == NULL) {
                    return NULL;
                }
                find->pattern = dbllist_data(temp);
                find->start_pos = i - strlen(find->pattern) + 1;
                find->end_pos = i;
                find->fstate = tState;
                dbllist_append(searchList,find);
                printf("pattern: %s ,start at: %d ,ends at: %d ,last state: %d\n", find->pattern, find->start_pos, find->end_pos, tState->id);
                temp = dbllist_next(temp);
            }
        }
        state = tState;
    }

    return searchList;
}


void pm_destroy(pm_t *fsmTree){
    if(fsmTree==NULL){ return;}//if not allocated return

    //Free all states in the FSM
    pm_state_t *temp_state=fsmTree->zerostate;//set a temp state to the zero state
    while(temp_state!=NULL){
        //Free the data in the output list of the state
        dbllist_destroy(temp_state->output,DBLLIST_FREE_DATA);
        //Free the data in the transitions list of the state
        dbllist_destroy(temp_state->_transitions,DBLLIST_FREE_DATA);
        //Free the state itself
        pm_state_t *next_state=temp_state->fail;//set a temp variable to the fail state of the current state
        free(temp_state);//free the current state
        temp_state=next_state;//set temp state to the next state
    }

}