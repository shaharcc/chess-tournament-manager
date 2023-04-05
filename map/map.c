/* map data structure */

#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define INITIAL_SIZE 10
#define EXPAND_FACTOR 2
#define ELEMENT_NOT_FOUND -1

typedef struct {
    MapDataElement data;
    MapKeyElement key;
} Element;

struct Map_t {
    int size;
    int max_size;
    int iterator;
    Element* elements;
    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
};

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements) {
                  if(copyDataElement == NULL || copyKeyElement == NULL || freeDataElement == NULL ||
                    freeKeyElement == NULL || compareKeyElements == NULL)
                        return NULL;

                   Map map = malloc(sizeof(*map));
                    if(map == NULL){
                        printf("Dynamic Allocation Error");
                        return NULL;
                    }
                        
                    map->size = 0;
                    map->max_size=INITIAL_SIZE;
                    map->elements = malloc(sizeof(Element)*INITIAL_SIZE);
                    if(map->elements == NULL)
                    {
                        free(map);
                        return NULL;
                    }
                    map->copyDataElement = copyDataElement;
                    map->copyKeyElement = copyKeyElement;
                    map->freeDataElement = freeDataElement;
                    map->freeKeyElement = freeKeyElement;
                    map->compareKeyElements = compareKeyElements;

                    return map;
              }

void mapDestroy(Map map) {
    if(map == NULL)
        return;
    mapClear(map);
    free(map->elements);
    free(map);
}

Map mapCopy (Map map) {
    if (map == NULL)
        return NULL;
    Map new_map = mapCreate(map->copyDataElement, map->copyKeyElement, map->freeDataElement,
                            map->freeKeyElement, map->compareKeyElements);
    if (new_map == NULL) {
        printf("Dynamic Allocation Error");
        return NULL;
    }
    for (int i = 0; i < map->size; i++) {
        new_map->elements[i].data = map->copyDataElement(map->elements[i].data);
        new_map->elements[i].key = map->copyKeyElement(map->elements[i].key);
    }
    new_map->size = map->size;
    new_map->max_size = map->max_size;
    new_map->copyDataElement = map->copyDataElement;
    new_map->copyKeyElement = map->copyKeyElement;
    new_map->freeDataElement = map->freeDataElement;
    new_map->freeKeyElement = map->freeKeyElement;
    new_map->compareKeyElements = map->compareKeyElements;

    return new_map;
}

int mapGetSize(Map map) {
    if(map == NULL) {
        return -1;
    }
    return map->size;
}

bool mapContains(Map map, MapKeyElement element) {
    for(int i = 0; i < map->size; i++) {
         if(map->compareKeyElements(map->elements[i].key, element) == 0)
            return true;
    }
    return false;
}

static int find(Map map, MapKeyElement keyElement) {
    for(int i = 0; i < map->size; i++) {
        if(map->compareKeyElements(map->elements[i].key, keyElement) == 0)
            return i;
    }
    return -1;
}

static MapResult expand(Map map) {
    int new_size = EXPAND_FACTOR*(map->size);
    Element* new_elements = realloc(map->elements, new_size*sizeof(*map));
    if(new_elements == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->elements = new_elements;
    map->max_size = new_size;
    return MAP_SUCCESS;
}

static int findCorrectIndex(Map map, MapKeyElement keyElement) {
    if(map == NULL || keyElement == NULL)
        return 0; // i.e. this is the first element
    int i;
    for(i = 0; i < map->size; i++)  {
        if(map->compareKeyElements(map->elements[i].key, keyElement) >= 0)
            return i;
    }
    return map->size;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    if(map == NULL || keyElement == NULL || dataElement == NULL)
        return MAP_NULL_ARGUMENT;
    
    if(mapContains(map, keyElement)) {
        // in case we need to replace data element associated with a given key
        int data_to_replace = find(map, keyElement);
        map->freeDataElement(map->elements[data_to_replace].data);
        map->elements[data_to_replace].data = map->copyDataElement(dataElement);
        if(map->elements[data_to_replace].data == NULL){
            return MAP_OUT_OF_MEMORY;
        }
    }
    else {
        // in case we need to insert a new element
        if(map->size == map->max_size) {
            expand(map);
        }
        int const correct_index = findCorrectIndex(map, keyElement);
        if(correct_index == map->size) {
            map->size++;
            map->elements[correct_index].key = map->copyKeyElement(keyElement);
            map->elements[correct_index].data = map->copyDataElement(dataElement);
        }
        else {
        // reordering the array to make room for the new element in it's right place
        for(int i = map->size-1; i >= correct_index; i--) {
            map->elements[i+1].key = map->elements[i].key;
            map->elements[i+1].data = map->elements[i].data;
        }
        map->size++;
        map->elements[correct_index].key = map->copyKeyElement(keyElement);
        map->elements[correct_index].data = map->copyDataElement(dataElement);
        }
    }
    return MAP_SUCCESS;
}


MapDataElement mapGet(Map map, MapKeyElement keyElement) {
    if (map == NULL || keyElement==NULL)
        return NULL;
    if (mapContains(map, keyElement) == false)
        return NULL;
    
    int i = find(map, keyElement);
    if (i == -1) {
        return NULL;
    }
    return map->elements[i].data;
}


MapResult mapRemove(Map map, MapKeyElement keyElement ){
    if(map == NULL || keyElement == NULL)
        return MAP_NULL_ARGUMENT;
    if(!mapContains(map, keyElement))
        return MAP_ITEM_DOES_NOT_EXIST;
    // the element exist in the map

    int element_to_remove = find(map, keyElement);
    map->freeDataElement(map->elements[element_to_remove].data);
    map->freeKeyElement(map->elements[element_to_remove].key);
    // element has been removed, now reordering the array

    int i;
    for(i = element_to_remove+1; i < map->size; i++) {
        map->elements[i-1] = map->elements[i];
    }
    map->freeDataElement(map->elements[i].data);
    map->freeKeyElement(map->elements[i].key);
    // frees the last element of the array
    map->size--;
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map) {
    if (map == NULL || map->size == 0) {
        return NULL;
    }
    map->iterator = 0;
    MapKeyElement first_key_copy = map->copyKeyElement(map->elements[map->iterator].key);
    return first_key_copy; 
}

MapKeyElement mapGetNext(Map map) {
    if (map == NULL) {
        return NULL;
    }
    if (map->iterator >= map->size-1) {
        return NULL;
    }
    map->iterator++;
    MapKeyElement key_copy = map->copyKeyElement(map->elements[map->iterator].key);
    return key_copy; 
}

MapResult mapClear(M ap map) {
    if (map == NULL){
        return MAP_NULL_ARGUMENT;
    }
    for (int i=0; i < map->size; i++) {
        map->freeDataElement(map->elements[i].data);
        map->freeKeyElement(map->elements[i].key);
    }
    return MAP_SUCCESS;
}
