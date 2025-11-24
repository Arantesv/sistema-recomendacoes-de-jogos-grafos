/*******************************************************************************
 *
 * AUTORES:
 * Andre Moreira Guimaraes - RA: 10416590
 * Leonardo Patriani Cardoso - RA: 10417188
 * Vitor Arantes Vendramini - RA: 10417759
 *
 * TURMA: 06G
 *
 * DESCRIÇÃO:
 * Arquivo principal da aplicação (main). Contém o menu de interação com o
 * usuário, o loop principal do programa e a chamada para as funções de
 * manipulação do grafo definidas em Grafo.c.
 *
 * HISTÓRICO DE ALTERAÇÕES:
 * 19/09/2025, Andre Guimaraes: Estrutura inicial do menu e loop principal.
 * 20/09/2025, Vitor Arantes: Adição de todas as opções do menu (a-j) e integração com
 * as funções do grafo.
 * 21/09/2025, Leonardo Patriani: Melhoria da interface com busca por ID ou nome.
 * 28/09/2025, Vitor Arantes e Andre Guimaraes: Limpeza final e ajuste dos textos do menu.
 *
 * COMO COMPILAR:
 * gcc main.c TGrafoLista.c -o programa_grafos
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TGrafoLista.h"

void exibirMenu()
{
    printf("\n===== MENU - PROJETO TEORIA DOS GRAFOS =====\n");
    printf(" a) Ler dados do arquivo 'grafo.txt'\n");
    printf(" b) Gravar dados no arquivo 'grafo.txt'\n");
    printf(" c) Inserir vértice (jogo)\n");
    printf(" d) Inserir aresta (similaridade)\n");
    printf(" e) Remover vértice (jogo)\n");
    printf(" f) Remover aresta (similaridade)\n");
    printf(" g) Mostrar conteúdo do arquivo\n");
    printf(" h) Mostrar grafo\n");
    printf(" i) Apresentar a conexidade do grafo\n");
    printf(" j) Recomendar jogos similares\n");
    printf(" k) Analisar grau dos vértices\n");
    printf(" l) Verificar se é Euleriano\n");
    printf(" m) Verificar Teoremas Hamiltonianos\n");
    printf(" n) Encerrar a aplicação\n");
    printf("============================================\n");
    printf(" Escolha uma opção: ");
}

int main()
{
    Grafo *grafo = NULL;
    char opcao;
    const char *nome_arquivo = "grafo.txt";

    do
    {
        exibirMenu();
        scanf(" %c", &opcao);
        while (getchar() != '\n')
            ;

        if (opcao != 'a' && opcao != 'g' && opcao != 'k' && grafo == NULL)
        {
            printf("\nERRO: Por favor, carregue um grafo primeiro (opção 'a').\n");
            continue;
        }

        switch (opcao)
        {
        case 'a':
            if (grafo)
            {
                liberaGrafo(grafo);
                free(grafo);
            }
            grafo = lerArquivo(nome_arquivo);
            if (grafo)
                printf("\nGrafo carregado com %d jogos.\n", grafo->num_vertices);
            break;

        case 'b':
            gravarArquivo(nome_arquivo, grafo);
            break;

        case 'c':
        {
            char buffer_rotulo[100];
            printf("Digite o nome do novo jogo: ");
            fgets(buffer_rotulo, 100, stdin);
            buffer_rotulo[strcspn(buffer_rotulo, "\n")] = 0;
            insereVertice(grafo, buffer_rotulo);
            break;
        }

        case 'd':
        {
            printf("Primeiro jogo (ID ou Nome): ");
            int id_origem = obtemIdPorEntrada(grafo);
            if (id_origem == -1)
            {
                printf("Primeiro jogo não encontrado.\n");
                break;
            }

            printf("Segundo jogo (ID ou Nome): ");
            int id_destino = obtemIdPorEntrada(grafo);
            if (id_destino == -1)
            {
                printf("Segundo jogo não encontrado.\n");
                break;
            }

            int peso;
            printf("Digite a similaridade (1-10): ");
            scanf("%d", &peso);
            while (getchar() != '\n')
                ;

            if (id_origem == id_destino)
            {
                printf("Não é possível criar uma aresta para o mesmo vértice.\n");
            }
            else
            {
                insereAresta(grafo, id_origem, id_destino, peso);
                printf("Aresta inserida com sucesso.\n");
            }
            break;
        }

        case 'e':
        {
            printf("Digite o ID ou NOME do jogo a ser removido: ");
            int id_remover = obtemIdPorEntrada(grafo);
            if (id_remover != -1)
            {
                removeVertice(grafo, id_remover);
            }
            else
            {
                printf("Erro: Jogo não encontrado.\n");
            }
            break;
        }

        case 'f':
        {
            printf("Primeiro jogo da aresta (ID ou Nome): ");
            int id_origem = obtemIdPorEntrada(grafo);
            if (id_origem == -1)
            {
                printf("Primeiro jogo não encontrado.\n");
                break;
            }

            printf("Segundo jogo da aresta (ID ou Nome): ");
            int id_destino = obtemIdPorEntrada(grafo);
            if (id_destino == -1)
            {
                printf("Segundo jogo não encontrado.\n");
                break;
            }

            removeAresta(grafo, id_origem, id_destino);
            printf("Aresta removida com sucesso.\n");
            break;
        }

        case 'g':
            mostraConteudoArquivo(nome_arquivo);
            break;

        case 'h':
            mostraGrafo(grafo);
            break;

        case 'i':
            apresentaConexidade(grafo);
            break;

        case 'j':
        {
            printf("Digite o ID ou NOME do jogo que você jogou: ");
            int id_jogo = obtemIdPorEntrada(grafo);
            if (id_jogo != -1)
            {
                obtemRecomendacoes(grafo, id_jogo);
            }
            else
            {
                printf("Erro: Jogo não encontrado.\n");
            }
            break;
        }
        case 'k':
            apresentaGraus(grafo);
            break;

        case 'l':
            verificaEuleriano(grafo);
            break;

        case 'm':
            verificarTeoremasHamiltonianos(grafo);
            break;

        case 'n':
            printf("Encerrando a aplicação...\n");
            break;

        default:
            printf("Opção inválida. Tente novamente.\n");
            break;
        }

    } while (opcao != 'k');

    if (grafo)
    {
        liberaGrafo(grafo);
        free(grafo);
    }

    return 0;
}