using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using BotTemplate.Core.NaturalLanguage;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class DuvidaPisadaDialog : IDialog
    {
        private readonly NaturalLanguageResult _result;

        public DuvidaPisadaDialog(NaturalLanguageResult result)
        {
            _result = result;
        }
        public async Task StartAsync(IDialogContext context)
        {

            DadosGerais.Dados.TryGetValue("genero", out var genero);
            DadosGerais.Dados.TryGetValue("pisada", out var pisada);
            DadosGerais.Dados.TryGetValue("objetivo", out var objetivo);
            if (genero != null && pisada != null && objetivo != null)
            {
                context.Call(new SugestaoProdutoDialog(), DadosGerais.EmptyResult);
            }
            else if (genero == null)
            {
                context.Call(new ProcuraProdutoDialog(_result), DadosGerais.EmptyResult);
            }
            else if (pisada == null)
            {
                await context.PostAsync("Entendi, e qual o tipo da sua pisada?");
                var replyMessage = context.MakeMessage();
                replyMessage.AttachmentLayout = AttachmentLayoutTypes.Carousel;
                replyMessage.Attachments = this.PisadaCard();
                await context.PostAsync(replyMessage);
                context.Wait(EscolheuPisada); 
            }
            else if (objetivo == null)
            {
                context.Call(new ObjectivoCorridaDialog(_result), DadosGerais.EmptyResult);
            }
            else
            {
                context.Done("");
            }
        }

        private async Task EscolheuPisada(IDialogContext context, IAwaitable<IMessageActivity> result)
        {
            var activity = await result as Activity;

            var data = activity.Text;
            await context.PostAsync(data);

            DadosGerais.Dados.Remove("pisada");
            DadosGerais.Dados.Add("pisada", data);
            context.Call(new ObjectivoCorridaDialog(_result), this.StartouObjetivoCorrida);
        }


        private async Task StartouObjetivoCorrida(IDialogContext context, IAwaitable<object> result)
        {

            context.Done("");
        }

        private List<Attachment> PisadaCard()
        {
            List<Attachment> attachments = new List<Attachment>();
            attachments.Add(new HeroCard()
            {
                Title = "Pronada",
                Subtitle = "Quando a parte de fora do calcanhar toca no chão",
                Text = "o pé inicia uma rotação excessiva para dentro",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/pronada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Pronada", value: "Pronada") }
            }.ToAttachment());

            attachments.Add(new HeroCard()
            {
                Title = "Supinada",
                Subtitle = "o pé toca o solo com a face externa do calcanhar",
                Text = "o pé toca o solo com a face externa do calcanhar",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/Supinada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Supinada", value: "Supinada") }
            }.ToAttachment());


            attachments.Add(new HeroCard()
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