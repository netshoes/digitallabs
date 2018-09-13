using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;
using Microsoft.Bot.Builder.Dialogs;

namespace BotTemplate.Dialogs
{
    static public class DadosGerais
    {
        public static Dictionary<string, object> Dados = new Dictionary<string, object>();
        static public async Task EmptyResult(IDialogContext context, IAwaitable<object> result)
        {
        }
    }
}