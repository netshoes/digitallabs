using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using BotTemplate.Core.Azure;
using BotTemplate.Core.NaturalLanguage;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    public static class extensoes
    {
        public static string ToUtf8(this string value)
        {
            var bytes = Encoding.Default.GetBytes(value);
            return Encoding.UTF8.GetString(bytes);
        }
    }
    [Serializable]
    public class ObjectivoCorridaDialog : IDialog
    {
        private readonly NaturalLanguageResult _result;

        public ObjectivoCorridaDialog(NaturalLanguageResult result)
        {
            _result = result;
        }

        public async Task StartAsync(IDialogContext context)
        {
            DadosGerais.Dados.TryGetValue("genero", out var genero);
            DadosGerais.Dados.TryGetValue("pisada", out var pisada);
            DadosGerais.Dados.TryGetValue("objetivo", out var objetivo);
            if (genero == null)
            {
                context.Call(new ProcuraProdutoDialog(_result), DadosGerais.EmptyResult);
            }
            else if (pisada == null)
            {
                context.Call(new DuvidaPisadaDialog(_result), DadosGerais.EmptyResult);
            }
            else if (objetivo == null)
            {
                await context.PostAsync("Certo! E qual o seu objetivo?");
                var replyMessage = context.MakeMessage();
                replyMessage.AttachmentLayout = AttachmentLayoutTypes.Carousel;
                replyMessage.Attachments = this.ObjetivoCard();
                await context.PostAsync(replyMessage);
                context.Wait(EscolheuObjetivo);
            }
            else
            {
                context.Done("");
            }
            
        }
        
        private async Task EscolheuObjetivo(IDialogContext context, IAwaitable<IMessageActivity> result)
        {
            var activity = await result as Activity;
            await context.PostAsync(activity.Text);

            DadosGerais.Dados.Remove("objetivo");
            DadosGerais.Dados.Add("objetivo", activity.Text);

            AzureCosmosDBClient mongo = new AzureCosmosDBClient();

            DadosGerais.Dados.TryGetValue("genero", out var genero);
            DadosGerais.Dados.TryGetValue("pisada", out var pisada);
            DadosGerais.Dados.TryGetValue("objetivo", out var objetivo);
            DadosGerais.Dados.TryGetValue("produto", out var produto);
            DadosGerais.Dados.TryGetValue("cor", out var cor);

            var dados = await mongo.GetProdutoByThings((string)genero, (string)pisada, (string)objetivo, (string)produto, (string)cor);

            List<Attachment> atts = new List<Attachment>();
            foreach (var prod in dados)
            {
                atts.Add(new HeroCard()
                {
                    Title = prod.name.ToUtf8(),
                    Images = new List<CardImage> { new CardImage($"https://static.netshoes.com.br/{prod.images.image}") },
                    Buttons = new List<CardAction>() { new CardAction(ActionTypes.OpenUrl, "Ir Até pagina do produto", value: "https://netshoes.azurewebsites.net/pdp.html") }
                }.ToAttachment());
            }

            var replyMessage = context.MakeMessage();
            replyMessage.AttachmentLayout = AttachmentLayoutTypes.Carousel;
            replyMessage.Attachments = atts;
            if (atts.Count > 0)
            {
                await context.PostAsync("Aqui estão alguns produtos que acredito que você vai gostar:");
                await context.PostAsync(replyMessage);
            }
            else
            {
                dados = await mongo.GetProdutoByThings((string)genero, null, null, null, null, true);

                atts = new List<Attachment>();
                foreach (var prod in dados)
                {
                    atts.Add(new HeroCard()
                    {
                        Title = prod.name.ToUtf8(),
                        Images = new List<CardImage> { new CardImage($"https://static.netshoes.com.br/{prod.images.image}") },
                        Buttons = new List<CardAction>() { new CardAction(ActionTypes.OpenUrl, "Ir Até pagina do produto", value: $"https://www.netshoes.com.br/{prod.code}") }
                    }.ToAttachment());
                }

                replyMessage = context.MakeMessage();
                replyMessage.AttachmentLayout = AttachmentLayoutTypes.Carousel;
                replyMessage.Attachments = atts;
                await context.PostAsync("Que tal algum desses modelos:");
                await context.PostAsync(replyMessage);
            }

            context.Wait(Whatever);
        }

      
        private async Task Whatever(IDialogContext context, IAwaitable<IMessageActivity> result)
        {
            context.Done("");
        }

        private List<Attachment> ObjetivoCard()
        {
            List<Attachment> attachments = new List<Attachment>();
            attachments.Add(new HeroCard()
            {
                Title = "Fazer Trilhas",
                Images = new List<CardImage> { new CardImage("http://www.anitaacontece.com.br/wp-content/uploads/2015/04/calcado-para-trilha.jpg") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Quero fazer trilhas", value: "trilha") }
            }.ToAttachment());

            attachments.Add(new HeroCard()
            {
                Title = "Correr na cidade",
               Images = new List<CardImage> { new CardImage("https://image.freepik.com/fotos-gratis/cimento-rua-financeiro-centro-cidade-viagem-de-xangai_1417-886.jpg") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Quero correr em lugares pavimentados", value: "pavimentacao") }
            }.ToAttachment());
            

            return attachments;
        }
    }
}