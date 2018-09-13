using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;
using System.Net;
using System.IO;
using Newtonsoft.Json;
using Microsoft.Bot.Connector;
using BotTemplate.Core.NaturalLanguage;
using System.Linq;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class DicasEsporteDialog : IDialog
    {
        const string accessKey = "c3d513c8d1e240ab820948f40ba69d0e";
        
        const string uriBase = "https://api.cognitive.microsoft.com/bing/v7.0/news/search";
        private List<NaturalLanguageEntity> entities;

        public DicasEsporteDialog(List<Core.NaturalLanguage.NaturalLanguageEntity> entities)
        {
            this.entities = entities;
        }

        public async Task StartAsync(IDialogContext context)
        {
            var esporte = "";
            if (this.entities.Count > 0)
                esporte = this.entities.First().Entity;
            

            switch (esporte)
            {
                case "corrida":
                    await context.PostAsync("Opa ! aqui vai uma dica de " + esporte + ":");
                    Random rnd = new Random();
                    int numeroDica = rnd.Next(1, 6);
                    switch (numeroDica)
                    {
                        case 1:
                            await context.PostAsync("Os braços são tão importantes quanto as pernas na hora de correr. Parados demais, diminuem a mobilidade e atrapalham a postura; balançando demais, ou muito na frente do corpo, impedem que você alcance a melhor forma de correr. Ou seja, braços de corredor se movimentam, funcionam como pêndulos e ajudam o corpo a se impulsionar para a frente. Então, se você quer começar a correr: não se esqueça dos braços. ");
                            break;
                        case 2:
                            await context.PostAsync("O core é nosso centro de força, composto por 29 pares de músculos que suportam e estabilizam bacia, pélvis e o abdômen. Um core forte ajuda o atleta a aguentar o tranco da corrida, ajuda na postura e estabiliza o impacto que o esporte tem nas articulações. Além disso, ajuda na respiração.  Se você é iniciante na corrida, invista, sobretudo, em exercícios de prancha isométrica e abdominais . Há outras opções, como o superman e o mountain climber.");
                            break;
                        case 3:
                            await context.PostAsync("Todo corredor deveria agachar muito. E saltar também. Box, caixa, step, qualquer uma dessas opções que te faça trabalhar força e potência muscular. A força da impulsão dos membros inferiores e a estabilidade do core ajudam, e muito, o atleta a fortalecer o corpo e fugir de lesões na corrida. Agachar vai te deixar mais forte. Saltar vai te dar mais potência muscular para subir, descer ou acelerar. ");
                            break;
                        case 4:
                            await context.PostAsync("Faça treinos educativos. Eles podem até ser chatos de fazer. Mas ajudam (e muito!) a mecânica dos iniciantes na corrida. Além disso, trabalham os músculos utilizados na modalidade. Ou seja, não podem ser esquecidos. Houpserlauf, escadinha de mobilidade e corrida para trás são alguns exemplos de educativos extremamente úteis. O resultado vai te fazer começar a gostar deles.");
                            break;
                        case 5:
                        default:
                            await context.PostAsync("Se você é um iniciante na corrida, já teve ou tem essa dúvida: existe um só jeito de respirar direito? A resposta é não. Você pode usar o nariz ou a boca, de acordo com o que te deixar mais confortável. Mas é importante treinar a respiração, assim como os outros elementos envolvidos na corrida.");
                            break;
                    }
                    break;                
                default:
                    await context.PostAsync("Ainda não estou dando dicas disso.");
                    break;
            }
            
            context.Done("");
        }
        
    }
    
}