using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using BotTemplate.Core.Azure;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class DuvidaPisadaDialog : IDialog
    {
        private CosmosDBProduto ProdutoStorage;
        private string idProduto = "5b72d7d688cc4148841d31d6";
        private String tipoPisada;

        public DuvidaPisadaDialog()
        {
            ProdutoStorage = new CosmosDBProduto();
        }
                
        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("Entendi, podemos descobrir, mas preciso te fazer algumas perguntas! Qual o tipo da sua pisada?");
            var replyMessage = context.MakeMessage();
            replyMessage.AttachmentLayout = AttachmentLayoutTypes.Carousel;
            replyMessage.Attachments = this.PisadaCard();
            await context.PostAsync(replyMessage);
            context.Wait(EscolheuPisada);
        }

        private async Task EscolheuPisada(IDialogContext context, IAwaitable<IMessageActivity> result)
        {
            var activity = await result as Activity;
            
            this.tipoPisada = activity.Text;
            await context.PostAsync("Legal, então a sua pidade é "+this.tipoPisada);

            ProdutoTenis prod = ProdutoStorage.GetProdutoId(this.idProduto);
            if (prod.Pisada.Contains(this.tipoPisada))
            {
                await context.PostAsync("Foi mlk " + this.tipoPisada);

            }
        }

            private List<Attachment> PisadaCard()
        {
            List<Attachment> attachments = new List<Attachment>();
            attachments.Add(new ThumbnailCard()
            {
                Title = "Pronada",
                Subtitle = "Quando a parte de fora do calcanhar toca no chão",
                Text = "o pé inicia uma rotação excessiva para dentro",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/pronada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Pronada", value: "PRONADA") }
            }.ToAttachment());

            attachments.Add(new ThumbnailCard()
            {
                Title = "Supinada",
                Subtitle = "o pé toca o solo com a face externa do calcanhar",
                Text = "o pé toca o solo com a face externa do calcanhar",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/Supinada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Supinada", value: "SUPINADA") }
            }.ToAttachment());


            attachments.Add(new ThumbnailCard()
            {
                Title = "Neutra",
                Subtitle = "Começa com a parte externa do calcanhar",
                Text = "o pé rotaciona ligeiramente para dentro",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/Normal.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Neutra", value: "Neutra") }
            }.ToAttachment());

            return attachments;
        }


    }
}