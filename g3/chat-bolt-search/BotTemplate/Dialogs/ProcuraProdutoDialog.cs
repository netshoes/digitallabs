using System;
using System.Threading.Tasks;
using Microsoft.Bot.Connector;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class ProcuraProdutoDialog : IDialog
    {
        private readonly NaturalLanguageResult _result;
        public ProcuraProdutoDialog(NaturalLanguageResult result)
        {
            _result = result;
        }

        public async Task StartAsync(IDialogContext context)
        {

            _result?.Entities?.ForEach(entity =>
            {
                if (DadosGerais.Dados.ContainsKey(entity.Type.ToLower()))
                {
                    DadosGerais.Dados.Add(entity.Type.ToLower(), entity.Entity);
                }
            });

            DadosGerais.Dados.TryGetValue("genero", out var genero);
            DadosGerais.Dados.TryGetValue("pisada", out var pisada);
            DadosGerais.Dados.TryGetValue("objetivo", out var objetivo);
            if (genero != null && pisada != null && objetivo != null)
            {
                context.Call(new SugestaoProdutoDialog(), EmptyResult);
            }
            else if (genero == null)
            {
                await context.PostAsync("Certo, esse produto é para homem ou mulher?");
                context.Wait(HomemOuMulher);
            }
            else if (pisada == null)
            {
                context.Call(new DuvidaPisadaDialog(_result), EmptyResult);
            }
            else if (objetivo == null)
            {
                context.Call(new ObjectivoCorridaDialog(_result), EmptyResult);
            }
            else
            {
                context.Done("");
            }
        }

        private async Task HomemOuMulher(IDialogContext context, IAwaitable<IMessageActivity> message)
        {
            var messageActivity = await message;
            if (messageActivity.Text.Equals("mulher") || messageActivity.Text.Equals("homem"))
            {
                DadosGerais.Dados.Remove("genero");
                DadosGerais.Dados.Add("genero", messageActivity.Text);
                context.Call(new DuvidaPisadaDialog(_result), EmptyResult);
            }
            else
            {
                await context.PostAsync("Não entendi, pode informar novamente?");
                context.Wait(HomemOuMulher);
            }
        }


        private async Task EmptyResult(IDialogContext context, IAwaitable<object> result)
        {
            context.Done("");
        }

    }
}