#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int	ft_strlen(char *str)
{
	int	count = 0;
	while (str[count])
		count++;
	return (count);
}

void	cd(char **arg)
{
	if (arg[0] == NULL || arg[1] != NULL)
	{	
		write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
		return ;
	}
	int ret = chdir(arg[0]);
	if (ret != 0)
		write(2, "error: cd: cannot change directory to path_to_change\n",
			ft_strlen("error: cd: cannot change directory to path_to_change\n"));
}

void	execute(char **argv, char **envp, int *prev_fd, bool is_last)
{
	int	fd[2];
	pipe(fd);
	pid_t pid = fork();

	if (pid == 0)
	{
		if (*prev_fd != -1)
		{
			dup2(*prev_fd, STDIN_FILENO);
			close(*prev_fd);
		}
		if (!is_last)
		{
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
		}
		execve(argv[0], argv, envp);
		write(STDERR_FILENO, "error: cannot execute ", ft_strlen("error: cannot execute "));
		write(STDERR_FILENO, argv[0], ft_strlen(argv[0]));
		write(STDERR_FILENO, "\n", 1);
		//perror("execve");
		exit(1);
	}
	if (*prev_fd != -1)
		close(*prev_fd);
	if (!is_last)
	{
		close(fd[1]);
		*prev_fd = fd[0];
	}
	else
	{
		*prev_fd = -1;
		waitpid(pid, NULL, 0);
	}

}

int	main(int argc, char **argv, char **envp)
{
	int	ci = 0;
	int i = 1;
	int	prev_fd = -1;
	char *cmd[1000];

	if (argc == 1)
		return (1);
	while(i < argc)
	{
		if (strcmp(argv[i], "cd") == 0)
		{
			cd(&argv[i+1]);
			//printf("hi\n");
			while (argv[i] != NULL)
			{
				if (strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0)
					break;
				i++;
			}
			//printf("hi\n");
		}
		else if (strcmp(argv[i], "|") == 0)
		{
			cmd[ci] = NULL;
			execute(cmd, envp, &prev_fd, false);
			ci = 0;
		}
		else if (strcmp(argv[i], ";") == 0)
		{
			if (ci == 0)
			{
				i++;
				continue;
			}
			cmd[ci] = NULL;
			ci = 0;
			execute(cmd, envp, &prev_fd, true);
			//if (argv[i + 1] == NULL)
			//	execute(cmd, envp, &prev_fd, true);
			//else
			//	execute(cmd, envp, &prev_fd, false);
		}
		else if (argv[i + 1] == NULL)
		{
			cmd[ci++] = argv[i];
			cmd[ci] = NULL;
			execute(cmd, envp, &prev_fd, true);
		}
		else
		{
			cmd[ci] = argv[i];
			ci++;
		}
		i++;
	}
}

// メモ

/*
	エラー処理のメッセージ、次のコマンドまで飛ばす処理などを忘れがち
	条件分岐を増やしすぎて間違えている箇所がわかり辛い
	パイプのために前回のfdを持っていかないといけないwaitはwaitpidを使わないと待つ順番が
	前後してうまくいかないことがあるかも
*/

