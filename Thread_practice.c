#include <windows.h>
#include <stdio.h>

#define MAX_THREADS 32

// DWORD is an unsigned long int!
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-dtyp/262627d8-3418-4627-9218-4ffe110850b2
// typedef unsigned long DWORD, *PDWORD, *LPDWORD;

DWORD WINAPI worker()
{
	/* The function the threads use.
	 * States its ID and Simply starts at zero and loops up to ten.
	 *
	 */

	DWORD id;

	int i = 0;
	
	id = GetCurrentThreadId();

	printf("I am worker %d!\n", id);

	for(i=0; i < 10; i++)
	{
		printf("%d: %d\n", id, i);
	}

	return 0;
}

int main()
{
	DWORD ThreadId; 			// For the single thread.
	DWORD dwThreadIdArray[MAX_THREADS]; 	// Array for multiple thread IDs.

	//  typedef void* HANDLE
	HANDLE ThreadHandle;			// A single thread Handle.
	HANDLE hThreadArray[MAX_THREADS];	// Array for multiple thread handle.

	// Create a single thread that runs the worker function.
	ThreadHandle = CreateThread(NULL, 0, worker, NULL, 0, &ThreadId);

	// If the Handle is not a null, (function returned a value).
	if(ThreadHandle != NULL)
	{
		// Wait for the function to complete.
		WaitForSingleObject(ThreadHandle, INFINITE);

		// Close the Thread.
		CloseHandle(ThreadHandle);

		printf("Single Handle Closed\n");
	}

	// Loop up to the defined max.	
	for(int j=0; j<MAX_THREADS; j++)
	{
		// Place the handle into indexed array. Target the worker function.
		hThreadArray[j] = CreateThread(NULL, 0, worker, NULL, 0, &dwThreadIdArray[j]);

		// If create thread function returned a zero and print out.
		if(hThreadArray[j] == NULL)
		{
			printf("CreateThread Error.\n");

			return -1;
		}

	}

	// Wait until all threads are completed.
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// Close all threads handles in the array.
	for(int j=0; j<MAX_THREADS; j++)
	{
		CloseHandle(hThreadArray[j]);
	}

	return 0;
}

