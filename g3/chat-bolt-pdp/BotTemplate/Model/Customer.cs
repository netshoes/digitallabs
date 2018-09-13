using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace BotTemplate.Model
{
    [Serializable]
    public class Customer
    {

        public Customer(string cnpj, string name)
        {
            this.cnpj = cnpj;
            this.name = name;
        }

        public string cnpj { get; set; }

        public string name { get; set; }
    }
}