#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//*************************************
// * MACRO DEFINITIONS
//*************************************
// https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
// MACRO for printing error messages. Disabled when 0 and enabled when 1.
// ! THIS MUST BE DISABLED BEFORE SUBMISSION
#define PRINT_OUT 1 // * MODES: 0 - DISABLE, 1 - ENABLE
#define print_out(fmt, ...)                                          \
    do                                                               \
    {                                                                \
        if (PRINT_OUT)                                               \
            fprintf(stderr, "%s: " fmt "", __func__, ##__VA_ARGS__); \
    } while (0)
#define NUM_LINES 20000
#define INVALID_INPUT fprintf(stderr, "Invalid Input Format\n")

struct tweeter_info
{
    char *name;
    int count;
};

struct tweeter_info tweeter[NUM_LINES];
int next_avail_index;
int quotes_flag;

int getCommasCount(char *str)
{
    int count_commas = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ',')
        {
            count_commas++;
        }
    }
    return count_commas;
}

int wrappedInQuotations(char *str)
{
    if (str == NULL)
    {
        INVALID_INPUT;
        exit(1);
    }
    int length = strlen(str) - 1;
    if (length > 0 && str[0] == '\"' && str[length] == '\"')
    {
        return 0;
    }
    INVALID_INPUT;
    exit(1);
    return 0;
}


/* PROCESS LINE TO ARRAY OF STRINGS */
void procLn2ArrOfStrs(char *str, char **splitted_array)
{
    // If string is null, exit with return code 1
    if (str == NULL)
    {
        INVALID_INPUT;
        exit(1);
    }
    /* Below gives an array consisting of comma intervals */
    // Suppose string is of size n
    // i.e. [0, comma_location_1, comma_location_2, ..., comma_location_n, n-1]

    // pad the array with the starting index of the string - 0 in the front
    int alloc_size = 1;
    int *count_ptr = (int *)malloc(sizeof(int));
    *count_ptr = 0;
    int comma_cnt = 0; // keep a comma count

    // we keep reallocating since we are extending the array size for every comma found
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ',')
        {
            comma_cnt++;
            int *count_ptr_tmp = (int *)realloc(count_ptr, sizeof(int) * (alloc_size + 1));
            alloc_size++;
            count_ptr = count_ptr_tmp;
            *(count_ptr + alloc_size - 1) = i; // save the location of the comma
        }
    }
    // pad the array with the ending index of the string - (n-1) in the end
    int *count_ptr_tmp = (int *)realloc(count_ptr, sizeof(int) * (alloc_size + 1));
    count_ptr = count_ptr_tmp;
    *(count_ptr + alloc_size) = strlen(str) - 1;

    /* BELOW we begin making the splits, and storing them in splitted_array*/
    // splitted_array is an array of strings (a double pointer)

    // Indicator variable (need it because of this crappy design)
    int indicate = 0;
    // printf("%d\n", alloc_size);
    for (int i = 0; i < alloc_size; i++)
    {
        if (*(count_ptr + i) == *(count_ptr + i + 1))
        {
            splitted_array[i] = (char *)malloc(sizeof(char));
            if (splitted_array[i] == NULL)
            {
                fprintf(stderr, "Invalid input here\n");
                exit(0);
            }
            indicate = 1;
            // printf("Splitted Array: %s\n", splitted_array[i]);
            continue;
        }
        if (i > 0)
        {
            indicate = 1;
        }
        int curr_comma_index = *(count_ptr + i);
        int next_comma_index = *(count_ptr + i + 1);
        int size_to_alloc = next_comma_index - curr_comma_index - 1;

        splitted_array[i] = (char *)malloc(sizeof(char) * size_to_alloc);
        if (splitted_array[i] == NULL)
        {
            INVALID_INPUT;
            exit(0);
        }
        
        for (int j = curr_comma_index; j < next_comma_index - 1; j++)
        {
            // TWITTER USERNAME RULES, either a letter, a number, and underscore
            if (isalpha(str[j + indicate]) != 0 || isdigit(str[j + indicate]) != 0 || str[j + indicate] == '_' || str[j + indicate]  == '\"')
            {
                splitted_array[i][j - curr_comma_index] = str[j + indicate];
            } else {
                splitted_array[i][j - curr_comma_index] = '\0';
                // printf("%c", str[j + indicate]);
            }
            // printf("[%d,%d,%d], ", i, j, j-curr_comma_index);
        }
        // printf("Size to alloc: %d\n", size_to_alloc);
        // printf("Splitted Array: %s\n", splitted_array[i]);
        // printf("\n%d\n", *(count_ptr + i));
        // printf("-----------\n");
    }
    // free count_ptr
    free(count_ptr);
}

// comparator method to sort by count.
int comparator(const void *tw1, const void *tw2)
{
    struct tweeter_info *t1 = (struct tweeter_info *)tw1;
    struct tweeter_info *t2 = (struct tweeter_info *)tw2;

    return t1->count - t2->count;
}

int main(int argc, char const *argv[])
{
    FILE *file = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;
    size_t read;

    int name_col_index = -1;

    int num_of_cols;

    if (file == NULL)
    {
        INVALID_INPUT;
        return 0;
    }

    /* GET NAME COLUMN INDEX  */
    while ((read = getline(&line, &len, file)) != -1)
    {

        num_of_cols = getCommasCount(line) + 1;

        // HOW TO USE procLn2ArrOfStrs?
        char *header[num_of_cols]; // <---- 1) ALLOCATE AN ARRAY OF STRINGS LIKE THIS
        procLn2ArrOfStrs(line, header); // <---- 2) PASS IN THE LINE, AND ARRAY OF STRINGS to procLn2ArrOfStrs

        for (int i = 0; i < num_of_cols; i++)
        {
            if (strcmp(header[i], "name") == 0)
            {
                name_col_index = i;
                break;
            }
        }

        for (int i = 0; i < num_of_cols; i++)
        {
            if (strcmp(header[i], "\"name\"") == 0)
            {
                name_col_index = i;
                quotes_flag = 1;
                break;
            }
        }

        for (size_t i = 0; i < num_of_cols; i++)
        {
            if(header[i] != NULL) {
                free(header[i]);
            }
        }
        // if name column doesn't exist, then exit
        if (name_col_index == -1)
        {
            INVALID_INPUT;
            exit(1);
        }

        break;
    }

    /* BEGIN PROCESSING NAMES */
    // if text is empty, then 
    int j = 0;
    while ((read = getline(&line, &len, file)) != -1)
    {

        // Exit program if the # of column of line don't match the # of columns
        // of the current line
        if ((getCommasCount(line) + 1) != num_of_cols)
        {
            INVALID_INPUT;
            exit(0);
        }

        int userFound = -1;

        char *curr_line[num_of_cols];
        procLn2ArrOfStrs(line, curr_line);

        // Get current line username
        char *username = curr_line[name_col_index];
        size_t lengthOfUsername = strlen(username);

        printf("249: %s\n", username);
        if(quotes_flag == 1){
            if (wrappedInQuotations(username) != 0)
            {
                INVALID_INPUT;
                exit(1);
            }
        }
        printf("WORKED\n");
        // iterate through all `tweeter_info` struct and check if user already exists
        for (size_t i = 0; i < next_avail_index; i++)
        {

            if (strcmp(tweeter[i].name, username) == 0)
            {
                // if user exists, increase the count
                tweeter[i].count++;
                userFound = 1;
                break;
            }
        }

        // if user exists, move on to the next line after freeing memory
        if (userFound == 1)
        {
            for (size_t i = 0; i < num_of_cols; i++)
            {
                if (curr_line[i] != NULL)
                {
                    free(curr_line[i]);
                }
            }
            continue;
        }

        // if user not found, then create an entry in the `tweeter_info` array
        tweeter[next_avail_index].name = (char *)malloc(sizeof(char) * (lengthOfUsername + 1));

        if (tweeter[next_avail_index].name != NULL)
        {
            // printf("%s\n", pt);
            for (size_t i = 0; i < lengthOfUsername+1; i++)
            {
                // If some invalid text shows up due to some quirk of C and some memory issues occur
                if (isalpha(username[i]) != 0 || isdigit(username[i]) != 0 || username[i] == '_' || username[i] == '\"')
                {
                    tweeter[next_avail_index].name[i] = username[i];
                } else {
                    tweeter[next_avail_index].name[i] = '\0';
                }
            }
            username[lengthOfUsername] = '\0';
            
            // strncpy(tweeter[next_avail_index].name, username, lengthOfUsername+1);
            tweeter[next_avail_index].count++;
            next_avail_index++; // update the pointer that tracks the entries in the `tweeter_info` struct
        }

        // free memory that was incurred by reading the current line
        for (size_t i = 0; i < num_of_cols; i++)
        {
            if (curr_line[i] != NULL)
            {
                free(curr_line[i]);
            }
        }
        
    }

    // Check `texx.txt` file to see how the output looks like.
    // for (size_t i = 0; i < next_avail_index; i++)
    // {
    //     printf("Name: %s, Count: %d\n", tweeter[i].name, tweeter[i].count);
    // }

    /* TODO:
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
        !SORT tweeter[next_avail_index] based on next_avail_index[next_avail_index].count
        !and output the 10.
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
     */

    qsort(tweeter, next_avail_index, sizeof(struct tweeter_info), comparator);

    int minPossTweetrs = 9;
    if (next_avail_index <= minPossTweetrs)
    {
        for (int i = next_avail_index - 1; i >= 0; i--)
        {
            printf("%s: %d \n",
                   tweeter[i].name, tweeter[i].count);
        }
    }
    else
    {
        for (int i = next_avail_index - 1; i > next_avail_index - 11; i--)
        {
            printf("%s: %d \n",
                   tweeter[i].name, tweeter[i].count);
        }
    }

    // printf("\n\n(Just UPDATED now)Tweeters Record sorted by Tweet Count:\n");
    // for (int i = next_avail_index - 1; i > next_avail_index - 11; i--)
    // {
        // printf("%s: %d \n",
        //        tweeter[i].name, tweeter[i].count);
    // }

    fclose(file);
    return 0;
}
