import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule } from '@angular/forms';
import { ChartsModule } from 'ng2-charts/ng2-charts';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { LoginComponent } from './login/login.component';
import { AdminComponent } from './admin/admin.component';
import { FinancialDataComponent } from './financial-data/financial-data.component';
import { StockComponent } from './stock/stock.component';
import { PricingComponent } from './pricing/pricing.component';
import { TrackingComponent } from './tracking/tracking.component';
import { ContactComponent } from './contact/contact.component';
import { ConfirmModalComponent } from './confirm-modal/confirm-modal.component';

import { AuthGuard } from './services/auth.guard';
import { AuthenticationService } from './services/authentication.service';
import { UserService } from './services/user.service';
import { StockService } from './services/stock.service';
import { SpotService } from './services/spot.service';
import { DeltaService } from './services/delta.service';
import { PriceService } from './services/price.service';
import { YahooFinanceService } from './services/yahoo-finance.service';
import { PricerService } from './services/pricer.service';
import { FinancialDataService } from './services/financial-data.service';

import 'hammerjs';
import 'chartjs-plugin-zoom';

@NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    LoginComponent,
    AdminComponent,
    FinancialDataComponent,
    StockComponent,
    PricingComponent,
    TrackingComponent,
    ContactComponent,
    ConfirmModalComponent
  ],
  imports: [
    NgbModule.forRoot(),
    BrowserModule,
    HttpClientModule,
    FormsModule,
    ChartsModule,
    AppRoutingModule
  ],
  providers: [
    AuthGuard,
    AuthenticationService,
    UserService,
    StockService,
    SpotService,
    DeltaService,
    PriceService,
    YahooFinanceService,
    PricerService,
    FinancialDataService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
