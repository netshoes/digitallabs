using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class ComoFazerUmaTroca : IDialog
    {
        public object Tipo { get; private set; }

        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
        }

        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            await context.PostAsync($@"Você pode fazer a troca online em 3 passos. Confira:
Clique em “Meus Pedidos” e faça o login. Na lista, escolha o pedido e veja se ele já está disponível para troca, se estiver, é só escolher o produto e clicar em “Trocar item”.
Escolha o motivo nas opções exibidas. É importante, para os casos de defeito, descrever com detalhes a falha encontrada.Ao continuar, aparecerá a opção de troca, que pode ser pelo mesmo produto com outra cor ou tamanho, de acordo com a disponibilidade do estoque, ou vale-compra.
Depois, é só finalizar a troca e aguardar o e - mail enviado para o endereço cadastrado, com o código de postagem e outras orientações.      
");
            context.Done("");

        }

//        private async Task MessageReceivedAsync(IDialogContext context, IAwaitable<object> result)
//        {
//            var produto = new ProdutoMOCK() { Tipo = "Tenis para atividade X" };
//            var activity = await result as Activity;

//            await context.PostAsync($@"Me false sobre a troca, algum motivo específico para trocar o {produto.Tipo}? 
//Posso lhe sugerir alguns itens para ${produto.Tipo}");
//            await context.PostAsync($@"");
//            context.Wait(MessageReceivedAsync);
//        }

//        internal class ProdutoMOCK
//        {
//            public ProdutoMOCK()
//            {
//            }

//            public string Tipo { get; internal set; }
//        }

    }
}