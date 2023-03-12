
#include <malloc.h>
#include "slist.h"
void deleteNode(dbllist_t *list, dbllist_node_t *node);
void dbllist_init(dbllist_t *dbList){//Initialize a double linked list
 if(dbList==NULL){ return;}//if not allocated return
    dbllist_head(dbList)=NULL;// set head to null
    dbllist_tail(dbList)=NULL;//set tail to null
    dbllist_size(dbList)=0;// set size to null
}
void dbllist_destroy(dbllist_t *dbList,dbllist_destroy_t dbDealloc){
    if(dbList==NULL){ return;}//if not allocated return
    dbllist_node_t * temp=dbllist_head(dbList);//set a temp node to the head
    while(temp!=dbllist_tail(dbList)){//do while temp is not the tail of the list
        if(dbDealloc==DBLLIST_FREE_DATA){//check if to free data
            free(dbllist_data(temp));// free node data
        }
        temp=dbllist_next(temp);//set temp node to the next of the current node
        free(dbllist_head(dbList));// free head
        dbllist_head(dbList)=temp;// set head to the temp node
        dbllist_size(dbList)=dbllist_size(dbList)-1;// make the size less by 1
    }
    //Free the last node in the list
    if(dbDealloc==DBLLIST_FREE_DATA){
        if (temp!=NULL)
        free(dbllist_data(temp));// free node data
    }
    free(temp);// free the tail node
    //Set head and tail to NULL
    dbllist_head(dbList)=NULL;
    dbllist_tail(dbList)=NULL;
}


int dbllist_append(dbllist_t *dbList,void * dbData){//add as last node of the list
    if(dbList==NULL){   return -1;}// if list is null return -1
    dbllist_node_t *dbNode=(dbllist_node_t*) malloc(sizeof (dbllist_node_t));// allocate a node
    if (dbNode==NULL){return -1;}// if the node still null return -1
    void * n=dbData;

   dbllist_data( dbNode)= (n);// set data of the node to the data gotten
    if(dbllist_head(dbList)==NULL){//if the head is null then set given node to head
        dbllist_head(dbList)=dbNode;// set node to head
        dbllist_tail(dbList)=dbNode;//set node as tail
        dbllist_next(dbllist_tail(dbList))=NULL;// set next to null
        dbllist_prev(dbllist_head(dbList))=NULL;// set previous  to null
        dbllist_size(dbList)=(dbllist_size(dbList))+1;// add one to the size
    } else {// if the head is not null
        dbllist_node_t *temp;// set a temp
        temp = dbllist_tail(dbList);// set temp as the tail
        dbllist_data(temp) = dbllist_data(dbllist_tail(dbList));// set the data of the tail to the data of the tail
        dbllist_next(dbllist_tail(dbList)) = dbNode;// set the next of the tail to the node
        dbllist_tail(dbList) = dbNode;// set node as the tail of the list
        dbllist_next(dbNode) = NULL;// set the next to null
        dbllist_prev(dbllist_tail(dbList)) = temp;//set the temp as the previous
        dbllist_data(dbllist_prev(dbllist_tail(dbList))) = dbllist_data(temp);// set the data of the previous of the tail to the data of the temp
        dbllist_size(dbList) = dbllist_size(dbList) + 1;// add one to the size
    }
    return 0;
}
int dbllist_prepend(dbllist_t *dbList,void *dbData){//add as first node of the list
    if(dbList==NULL){ return -1;}// check if the list is null if it is return -1
    dbllist_node_t *dbNode=(dbllist_node_t*) malloc(sizeof (dbllist_node_t));// allocate a node
    if (dbNode==NULL){return -1;}// if the node is still null return -1
    void * n=dbData;

    dbllist_data( dbNode)= n;// set node data to given data
    if(dbllist_head(dbList)==NULL){// if the head is null
        dbllist_head(dbList)=dbNode;// set the head to the given node
        dbllist_tail(dbList)=dbNode;// set the tail to the given node
        dbllist_next(dbllist_tail(dbList))=NULL;// set next to null
        dbllist_prev(dbllist_head(dbList))=NULL;//set previous to null
        dbllist_size(dbList)=(dbllist_size(dbList))+1;// add one to the size of the list
    } else{// if the head is not null
        dbllist_node_t *temp;
        temp=dbllist_head(dbList);// set a temp as the head of the list
        dbllist_data(temp)=dbllist_data(dbllist_head(dbList));// set the data of the temp to the data of the head of the list
        dbllist_prev(dbllist_head(dbList))=dbNode;
        dbllist_head(dbList)=dbNode;// set the head as the given node
        dbllist_next(dbllist_head(dbList))=temp;// set the next to the head as the temp
        dbllist_data(dbllist_next(dbllist_head(dbList)))=dbllist_data(temp);// set the data of the next of the head as the data of the temp
        dbllist_prev(dbNode)=NULL;// set the previous to the node as null
        dbllist_size(dbList)=dbllist_size(dbList)+1;// add one to size
    }
    return 0;
}
int dbllist_remove(dbllist_t *dbList, dbllist_node_t* dbPointer,dbllist_destroy_t dbDealloc){
    if(dbList==NULL || dbPointer==NULL){ return -1;}// if the list or the pointer are null return -1

    // Update the next and previous pointers of the surrounding nodes to skip over the node to be removed
    if(dbllist_prev(dbPointer) != NULL){
        dbllist_next(dbllist_prev(dbPointer)) = dbllist_next(dbPointer);
    }
    if(dbllist_next(dbPointer) != NULL){
        dbllist_prev(dbllist_next(dbPointer)) = dbllist_prev(dbPointer);
    }

    // Update the head and tail pointers if necessary
    if(dbllist_head(dbList) == dbPointer){
        dbllist_head(dbList) = dbllist_next(dbPointer);
    }
    if(dbllist_tail(dbList) == dbPointer){
        dbllist_tail(dbList) = dbllist_prev(dbPointer);
    }

    // Free the data if necessary
    if(dbDealloc == DBLLIST_FREE_DATA){
        free(dbllist_data(dbPointer));
    }

    // Free the node
    free(dbPointer);
    dbllist_size(dbList)--;
    return 0;
}


