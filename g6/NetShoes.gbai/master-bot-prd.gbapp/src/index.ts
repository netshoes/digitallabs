"use strict"

import { IGBPackage, GBMinInstance, IGBCoreService } from 'botlib'
import { Sequelize } from 'sequelize-typescript'

export class Package implements IGBPackage {
  sysPackages: IGBPackage[]

  loadPackage(core: IGBCoreService, sequelize: Sequelize): void {
    
  }

  unloadPackage(core: IGBCoreService): void {

  }

  loadBot(min: GBMinInstance): void {
    
  }

  unloadBot(min: GBMinInstance): void {

  }

  onNewSession(min: GBMinInstance, dc: any): void {

  }
}
