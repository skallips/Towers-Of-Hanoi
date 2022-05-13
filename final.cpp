#include<string>
#include<vector>
#include<iostream>
#include<stdlib.h>  /* to use exit */
using namespace std;
//testing commit
//testing 2nd commit for noticeable change
//testing commit from my computer locally through git

class state
{
public:
    string items;  // made up of discs going from one column to another 
    string camefrom; // the parent on the solution path
    int g = 0;   // cost so far
    int h = 0;   // estimated cost to the goal
    int f = 0;   // g+h

    //funciton to check to see which column a particular disc is in and
    //returns string version of the column
    string whereIs(char x) {
        char result;

        //go through items and find position of item to be found
        for (int i = 0; i < 9; i++) {
            if (this->items[i] == x)
                result = i;
        }

        if ((result >= 0) && (result < 3))  
            return "left";
            
        if ((result >= 3) && (result < 6)) 
            return "middle";
            
        if (((result >= 6) && (result < 9))) 
            return "right";
            
        return "error finding element";
    }

    //function to draw out visualization of discs in columns
    void draw() {
        cout << "| " << this->items[0] << " | " << this->items[3] << " | " << this->items[6] << " |" << endl;
        cout << "| " << this->items[1] << " | " << this->items[4] << " | " << this->items[7] << " |" << endl;
        cout << "| " << this->items[2] << " | " << this->items[5] << " | " << this->items[8] << " |" << endl;
    }

};

// Major data structures:
vector<state> frontier;  // Frontier nodes
vector<state> beenThere; // already expanded nodes

// ========== Utility Functions ===================

// utility to add x to the beenthere
void addtoBeenThere(state x)
{
    beenThere.push_back(x);
}

// utility to add x to the frontier
void addtoFrontier(state x)
{
    frontier.push_back(x);
}

// to remove state x from the frontier
void removefromFrontier(state x)
{
    vector<state> newfrontier;
    for (int k = 0; k < frontier.size(); k++)
        if (frontier[k].items != x.items)
            newfrontier.push_back(frontier[k]);

    frontier = newfrontier;
}

//======= For Generating a new state  =============

// Trace and display the solution path from goal to initial.
// Note that camefrom of each state should be used. (called from generate)
void tracePath(state goal)
{
    //for storing previous state items
    state previousState;

    //seach the beenThere vector for previous matching items
    for (int k = 0; k < beenThere.size(); k++) {
        if (goal.camefrom == "") {
            return; //base case (start of game)
        }

        //if match found, assign the data into previous state and recall tracepath
        if (goal.camefrom == beenThere[k].items) {
            previousState = beenThere[k];
            //cout << "Came from: " << previousState.items << endl;
            cout << "Came from: " << endl;
            previousState.draw();

            tracePath(previousState);
            return;
        }
    }

    //if no path was complete, exit
    return;
}// end of tracePath 


// Check to see if next's items is already in frontier and return true or false.
//   If it is already in frontier, 
//   and if the next's f is better, 
//   update the frontier node to next.  (called from generate)
bool inFrontier(state next)
{
    //go through frontier vector
    for (int k = 0; k < frontier.size(); k++)
    {
        //check to see if match is found
        if (next.items == frontier[k].items) {
            //if the current f is better than what is in vector, update
            if (next.f < frontier[k].f) {
                frontier[k].f = next.f;
                cout << "State is in frontier but had a beffer F." << endl;
            }
            return true;    //match found
        }
    }
    return false;
}// end of inFrontier

//Utility function to check if state is unsafe.  This will check each column and make
//sure that each disc is sitting on top of a larger disc.  If there is a larger disc on
//top of a smaller disc, it will flag as an unsafe state.
bool isUnsafe(state checker) {

    //first column check 
    if ((checker.items[0] > checker.items[1]) || (checker.items[1] > checker.items[2]))
        return true;

    //second column check
    if ((checker.items[3] > checker.items[4]) || (checker.items[4] > checker.items[5]))
        return true;

    //third column check
    if ((checker.items[6] > checker.items[7]) || (checker.items[7] > checker.items[8]))
        return true;

    //state is safe
    return false;
}

// Try to add next to frontier but stop if LLLL (called from generateAll)
void generate(state next)
{
    cout << "Trying to add: " << endl;
    next.draw();

    if (next.items == "000000123")// the goal is reached
    {
        cout << ">>  Reached: " << next.items << endl;
        next.draw();
        tracePath(next);  // display the solution path
        cout << "Solution found, exiting program" << endl;
        exit(0);
    }//done

    //flag for checking other scenarios not to add to frontier
    bool donotadd = false;

    // *** if been there before, do not add to frontier.
    for (int k = 0; k < beenThere.size(); k++)
        if (beenThere[k].items == next.items)
            donotadd = true;

    // *** if unsafe, do not add to frontier.
    if (isUnsafe(next) == true) {
        cout << "Warning! The following is an unsafe state:" << endl;
        next.draw();
        donotadd = true;
    }

    // else compute h and then f for next
    // -----HERE, G is the number of moves that are made from one rod to another rod, and
    // H is the number of items still not on the last rod
    int h = 0;// start point for h; g is already set

    // ** update h and then f
    //This is checking all the positions in the first two columns.  If they are not a "zero", it
    //means there is still a disc not on the final side.  Increment H
    for (int i = 0; i < 6; i++) {
        if (next.items[i] != '0')
            h++;
    }
    next.f = next.g + h;
    next.h = h;

    if (!inFrontier(next) && donotadd == false)
        addtoFrontier(next); // add next to Frontier

    cout << endl;
}// end of generate 

// Generate all new states from current (called from main)
void generateAll(state current)
{
    state next;  // a generated one

   // each next will be modified from current for ease
    current.g = current.g + 1; // to update g to be used for next
    // storing the parent so that we can produce the solution path
    current.camefrom = current.items;

    // check all possibilities as follows..

    /*--MOVING THE FIRST DISC
        This will see which row the disc is in and move it.  Since it is the
        smallest disc, it can move freely and sit on top of any larger disc */
    next = current;

    //one is in left column, move to middle column
    if (current.whereIs('1') == "left") {
        //find position of 1 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 1
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //one is in left column, move to right column
    if (current.whereIs('1') == "left") {
        //find position of 1 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 1
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //one is in middle column, move to left column
    if (current.whereIs('1') == "middle") {
        //find position of 1 in the middle column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 1
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;
    //one is in middle column, move to right column
    if (current.whereIs('1') == "middle") {
        //find position of 1 in the middle column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 1
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //one is in right column, move to left column
    if (current.whereIs('1') == "right") {
        //find position of 1 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 1
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //one is in right column, move to middle column
    if (current.whereIs('1') == "right") {
        //find position of 1 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '1') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 1
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '1';
                break;
            }
        }
        generate(next);
    }

    next = current;

    /*----- MOVING THE SECOND DISC
            This will move the second smallest disc to another column.  This can
            only be done if the second disc is not in the same column as the first
            disc.  If a smaller disc is on top of the second disc, the second disc 
            is restricted from movement.  If the second disc is on top of a smaller 
            disc, it is an illegal state and cannot be moved.*/

    //two is in left column, move to middle column
    if ((current.whereIs('2') == "left") && (current.whereIs('1') != "left")) {
        //find position of 2 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 2
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //two is in left column, move to right column
    if ((current.whereIs('2') == "left") && (current.whereIs('1') != "left")) {
        //find position of 1 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 2
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //two is in middle column, move to left column
    if ((current.whereIs('2') == "middle") && (current.whereIs('1') != "middle")) {
        //find position of 2 in the middle column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 2
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //two is in middle column, move to right column
    if ((current.whereIs('2') == "middle") && (current.whereIs('1') != "middle")) {
        //find position of 2 in the left column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 2
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //two is in right column, move to left column
    if ((current.whereIs('2') == "right") && (current.whereIs('1') != "right")) {
        //find position of 2 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 2
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //two is in right column, move to middle column
    if ((current.whereIs('2') == "right") && (current.whereIs('1') != "right")) {
        //find position of 2 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '2') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 2
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '2';
                break;
            }
        }
        generate(next);
    }

    next = current;

    /*----- MOVING THE THIRD DISC
            This will move the largest disc to another column.  This can
            only be done if the largest disc is not in the same column as the first
            disc as well as the second disc.  If a smaller disc is on top of the 
            largest disc, the largest disc is restricted from movement.  If the 
            largest disc is on top of a smaller disc, it is an illegal state and 
            cannot be moved.*/

    //three is in left column, move to middle column
    if ((current.whereIs('3') == "left") && (current.whereIs('2') != "left") && (current.whereIs('1') != "left")) {
        //find position of 3 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 3
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //three is in left column, move to right column
    if ((current.whereIs('3') == "left") && (current.whereIs('2') != "left") && (current.whereIs('1') != "left")) {
        //find position of 1 in the left column and replace with 0
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 2
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //three is in middle column, move to left column
    if ((current.whereIs('3') == "middle") && (current.whereIs('2') != "middle") && (current.whereIs('1') != "middle")) {
        //find position of 3 in the middle column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 3
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //three is in middle column, move to right column
    if ((current.whereIs('3') == "middle") && (current.whereIs('2') != "middle") && (current.whereIs('1') != "middle")) {
        //find position of 3 in the left column and replace with 0
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in right column (bottom to top) and replace with 3
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //three is in right column, move to left column
    if ((current.whereIs('3') == "right") && (current.whereIs('2') != "right") && (current.whereIs('1') != "right")) {
        //find position of 3 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in left column (bottom to top) and replace with 3
        for (int i = 2; i > -1; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

    next = current;

    //three is in right column, move to middle column
    if ((current.whereIs('3') == "right") && (current.whereIs('2') != "right") && (current.whereIs('1') != "right")) {
        //find position of 3 in the right column and replace with 0
        for (int i = 8; i > 5; i--) {
            if (current.items[i] == '3') {
                next.items[i] = '0';
                break;
            }
        }
        //find first open spot in middle column (bottom to top) and replace with 3
        for (int i = 5; i > 2; i--) {
            if (current.items[i] == '0') {
                next.items[i] = '3';
                break;
            }
        }
        generate(next);
    }

}// end of generateAll

// Find the best f state out of the frontier and return it (called from main)
state bestofFrontier()
{
    state temp;
    temp.f = 100000; //set comparison mark as the first "f" in frontier using arbitrary number

    //go through frontier and find state with smallest f
    for (int k = 0; k < frontier.size(); k++)
        if (frontier[k].f < temp.f)
            temp = frontier[k];
    return temp;

}//end of bestofFrontier

// Display the states in the frontier  (called from main)
void displayFrontier()
{
    for (int k = 0; k < frontier.size(); k++)
    {
        cout << frontier[k].items << " ";
        cout << "g = " << frontier[k].g << " ";
        cout << "h = " << frontier[k].h << " ";
        cout << "f = " << frontier[k].f << endl;
    }
}// end of displayFrontier

int main() {
	cout << "Press any key to continue.  Press n to exit." << endl;
    state current;
    current.items = "123000000";    //beginning state, with all discs in order in left column
    current.camefrom = "";
    current.g = 0;
    current.h = 3;
    current.f = 3;
    addtoFrontier(current);

    char ans;
    cin >> ans;
    while (ans != 'n')
    {
        removefromFrontier(current);
        addtoBeenThere(current);

        cout << ">>>Expand:" << endl;
        current.draw();
        generateAll(current);  // new states are added to frontier

        cout << "Frontier is:" << endl;
        displayFrontier();

        current = bestofFrontier(); // pick the best state out of frontier
        cin >> ans;

    }// end while

	return 0;
}
