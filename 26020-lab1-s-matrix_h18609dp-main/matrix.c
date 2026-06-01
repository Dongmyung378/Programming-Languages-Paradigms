#include "matrix.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int matrix_allocate(matrix_t *m, int rows, int columns)
{
    m->rows = rows;
    m->columns = columns;
    m->content = (int **)calloc(rows, sizeof(int *));

    if (m->content == NULL)
    {
        return -1;
    }

    for (int i = 0; i < rows; i++)
    {
        m->content[i] = (int *)malloc(columns * sizeof(int));
        if (m->content[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(m->content[j]);
            }
            free(m->content);
            return -1;
        }
    }
    return 0;
}

void matrix_free(matrix_t *m)
{
    for (int i = 0; i < m->rows; i++)
    {
        free(m->content[i]);
    }

    free(m->content);
    m->rows = 0;
    m->columns = 0;
    m->content = NULL;
}

void matrix_init_n(matrix_t *m, int n)
{
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            m->content[i][j] = n;
        }
    }
}

void matrix_init_zeros(matrix_t *m)
{
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            m->content[i][j] = 0;
        }
    }
}

int matrix_init_identity(matrix_t *m)
{
    if (m->rows != m->columns)
    {
        return -1;
    }

    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            if (i == j)
            {
                m->content[i][j] = 1;
            }
            else
            {
                m->content[i][j] = 0;
            }
        }
    }
    return 0;
}

int matrix_init_rand(matrix_t *m, int val_min, int val_max)
{
    if (val_min > val_max)
    {
        return -1;
    }

    srand(time(NULL));

    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            m->content[i][j] = (rand() % (val_max - val_min + 1)) + val_min;
        }
    }
    return 0;
}

int matrix_equal(matrix_t *m1, matrix_t *m2)
{
    if (m1 == NULL || m2 == NULL || m1->content == NULL || m2->content == NULL)
    {
        return 0;
    }

    if (m1->rows != m2->rows)
    {
        return 0;
    }

    if (m1->columns != m2->columns)
    {
        return 0;
    }

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m1->columns; j++)
        {
            if (m1->content[i][j] != m2->content[i][j])
            {
                return 0;
            }
        }
    }

    return 1;
}

int matrix_sum(matrix_t *m1, matrix_t *m2, matrix_t *result)
{
    if (m1->rows != m2->rows || m1->columns != m2->columns)
    {
        return -1;
    }

    if (matrix_allocate(result, m1->rows, m1->columns) != 0)
    {
        return -1;
    }

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m1->columns; j++)
        {
            result->content[i][j] = m1->content[i][j] + m2->content[i][j];
        }
    }

    return 0;
}

int matrix_scalar_product(matrix_t *m, int scalar, matrix_t *result)
{
    if (m == NULL || m->content == NULL)
    {
        return -1;
    }

    if (matrix_allocate(result, m->rows, m->columns) != 0)
    {
        return -1;
    }

    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            result->content[i][j] = m->content[i][j] * scalar;
        }
    }

    return 0;
}

int matrix_transposition(matrix_t *m, matrix_t *result)
{
    if (matrix_allocate(result, m->columns, m->rows) != 0)
    {
        return -1;
    }

    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            result->content[j][i] = m->content[i][j];
        }
    }

    return 0;
}

int matrix_product(matrix_t *m1, matrix_t *m2, matrix_t *result)
{
    if (m1->columns != m2->rows)
    {
        return -1;
    }

    if (matrix_allocate(result, m1->rows, m2->columns) != 0)
    {
        return -1;
    }

    for (int i = 0; i < m1->rows; i++)
    {
        for (int j = 0; j < m2->columns; j++)
        {
            result->content[i][j] = 0;
            for (int k = 0; k < m1->columns; k++)
            {
                result->content[i][j] += m1->content[i][k] * m2->content[k][j];
            }
        }
    }

    return 0;
}

int matrix_dump_file(matrix_t *m, const char *output_file)
{
    FILE *file = fopen(output_file, "w");
    if (file == NULL)
    {
        return -1;
    }

    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->columns; j++)
        {
            if (j == m->columns - 1)
            {
                fprintf(file, "%d", m->content[i][j]);
            }
            else
            {
                fprintf(file, "%d ", m->content[i][j]);
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

int matrix_allocate_and_init_file(matrix_t *m, const char *input_file)
{
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        return -1;
    }

    int rows = 0;
    int columns = 0;
    char line[1024];

    if (fgets(line, sizeof(line), file) != NULL)
    {
        char *token = strtok(line, " ");
        while (token != NULL)
        {
            columns++;
            token = strtok(NULL, " ");
        }
        rows++;
    }
    else
    {
        fclose(file);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strlen(line) > 1)
        {
            rows++;
        }
    }

    if (matrix_allocate(m, rows, columns) != 0)
    {
        fclose(file);
        return -1;
    }

    rewind(file);
    for (int i = 0; i < rows; i++)
    {
        if (fgets(line, sizeof(line), file) != NULL)
        {
            char *token = strtok(line, " ");
            for (int j = 0; j < columns; j++)
            {
                if (token != NULL)
                {
                    m->content[i][j] = atoi(token);
                    token = strtok(NULL, " ");
                }
                else
                {
                    fclose(file);
                    return -1;
                }
            }
        }
        else
        {
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}