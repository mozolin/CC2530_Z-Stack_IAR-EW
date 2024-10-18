
// Function to add an element to an array of structs
extern zclAttrRec_t* addElement(zclAttrRec_t el,
  zclAttrRec_t arr[], int* counter,
  int size);
zclAttrRec_t* addElement(zclAttrRec_t el,
  zclAttrRec_t arr[], int* counter,
  int size)
{
  // Ensure array bounds are not exceeded
  if(*counter < size) {
    arr[*counter] = el;
    // Increment the count of element in the array
    (*counter)++;
    #if DEBUG_PRINT_UART
    	printf("counter=%d => %d\n", *counter, sizeof(*arr));
    #endif
  }
  else {
    #if DEBUG_PRINT_UART
    	printf("Array is full, cannot add more people.\n");
    #endif
  }
  return arr;
}

/*
// Function to add an element to an array of structs
void addElement(struct Person new_person,
                struct Person people[], int* counter,
                int size)
{
  // Ensure array bounds are not exceeded
  if(*counter < size) {
    people[*counter] = new_person;
    // Increment the count of element in the array
    (*counter)++;
  }
  else {
    #if DEBUG_PRINT_UART
    	printf("Array is full, cannot add more people.\n");
    #endif
  }
}
*/
