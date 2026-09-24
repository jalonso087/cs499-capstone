import { Component, OnInit, ChangeDetectorRef } from '@angular/core';
import { CommonModule} from '@angular/common';
//import { trips } from '../data/trips';
import { TripCardComponent } from '../trip-card/trip-card';
import { Trip } from '../models/trip';
import { TripDataService } from '../services/trip-data';
import { AuthenticationService } from '../services/authentication';

import { Router } from '@angular/router';

@Component({
  selector: 'app-trip-listing',
  providers: [TripDataService],
  imports: [CommonModule, TripCardComponent],
  templateUrl: './trip-listing.html',
  styleUrl: './trip-listing.css'
})
export class TripListingComponent implements OnInit {
  
  //trips: Array<any> = trips;

  trips!: Trip[];
  message: string = '';

  constructor(
    private tripDataService: TripDataService, 
    private cdr: ChangeDetectorRef,
    private authenticationService: AuthenticationService,
    private router: Router) {
    console.log('trip-listing constructor');
  }

  public addTrip(): void {
    this.router.navigate(['add-trip']);
  }

  public isLoggedIn() {
    return this.authenticationService.isLoggedIn();
  }

  private getStuff(): void {
    this.tripDataService.getTrips()
      .subscribe({
        next: (value: any) => {
          this.trips = value;

          this.cdr.detectChanges()

          if(value.length > 0)
          {
            this.message = 'There are ' + value.length + ' trips available.';
          } else {
              this.message = 'There were no trips retrieved from the database';
          }
          console.log(this.message);
        },
        error: (error: any) => {
          console.log('Error: ' + error);
        }
      })
  }

  ngOnInit(): void {
    console.log('ngOnInit');
    this.getStuff();
  }
}
