#include "../include/httpctrl.h"
static int status = 0;
int httpctrl()
{
	if (status == 1)
		pause();
	status = 1;
	//pause();
	//waitpid()
	//�ѹ����ڴ�ӵ�ǰ�����з���  
	//if (shmdt(shared) == -1)
	//{
	//	fprintf(stderr, "shmdt failed\n");
	//	exit(EXIT_FAILURE);
	//}

	//�ѹ����ڴ�ӵ�ǰ�����з���  
	//if (shmdt(shm) == -1)
	//{
	//	fprintf(stderr, "shmdt failed\n");
	//	exit(EXIT_FAILURE);
	//}
	////ɾ�������ڴ�  
	//if (shmctl(shmid, IPC_RMID, 0) == -1)
	//{
	//	fprintf(stderr, "shmctl(IPC_RMID) failed\n");
	//	exit(EXIT_FAILURE);
	//}
	return HTTP_RUN;
}